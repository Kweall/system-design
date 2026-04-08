#include "handlers/properties/post/handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/chrono.hpp>

namespace pillow {

PropertiesPostHandler::PropertiesPostHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context) {
    
    pg_cluster_ = context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster();
}

std::string PropertiesPostHandler::ExtractToken(const userver::server::http::HttpRequest& request) const {
    auto auth_header = request.GetHeader("Authorization");
    if (auth_header.empty() || auth_header.find("Bearer ") != 0) {
        return "";
    }
    return auth_header.substr(7);
}

bool PropertiesPostHandler::ValidateToken(const std::string& token, std::string& username, int64_t& user_id) const {
    if (token.find("jwt_") != 0) {
        return false;
    }
    
    std::string token_content = token.substr(4);
    auto underscore_pos = token_content.rfind('_');
    if (underscore_pos == std::string::npos) {
        return false;
    }
    
    username = token_content.substr(0, underscore_pos);
    
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id FROM users WHERE username = $1",
        username);
    
    if (result.IsEmpty()) {
        return false;
    }
    
    user_id = result[0][0].As<int64_t>();
    return true;
}

userver::formats::json::Value PropertiesPostHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext&) const {
    
    std::string token = ExtractToken(request);
    std::string username;
    int64_t user_id;
    
    if (token.empty() || !ValidateToken(token, username, user_id)) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Authentication required"});
    }
    
    if (!request_json.HasMember("title") || !request_json.HasMember("price") || 
        !request_json.HasMember("city") || !request_json.HasMember("rooms")) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Missing required fields: title, price, city, rooms"});
    }
    
    std::string title = request_json["title"].As<std::string>();
    std::string description = request_json.HasMember("description") ? 
                              request_json["description"].As<std::string>() : "";
    double price = request_json["price"].As<double>();
    std::string city = request_json["city"].As<std::string>();
    int rooms = request_json["rooms"].As<int>();
    
    if (title.empty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Title cannot be empty"});
    }
    
    if (price <= 0) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Price must be greater than 0"});
    }
    
    if (rooms < 1 || rooms > 10) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Rooms must be between 1 and 10"});
    }
    
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO properties (title, description, price, city, rooms, owner_id) "
        "VALUES ($1, $2, $3, $4, $5, $6) RETURNING id, created_at",
        title, description, price, city, rooms, user_id);
    
    int64_t property_id = result[0][0].As<int64_t>();
    auto created_at = result[0][1].As<std::chrono::system_clock::time_point>();
    
    userver::formats::json::ValueBuilder response;
    response["id"] = property_id;
    response["title"] = title;
    response["description"] = description;
    response["price"] = price;
    response["city"] = city;
    response["rooms"] = rooms;
    response["owner_id"] = user_id;
    response["created_at"] = std::to_string(std::chrono::system_clock::to_time_t(created_at));
    response["message"] = "Property created successfully";
    
    return response.ExtractValue();
}

} // namespace pillow
