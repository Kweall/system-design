#include "handlers/viewings/post/handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/chrono.hpp>

namespace pillow {

ViewingsPostHandler::ViewingsPostHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context) {
    
    pg_cluster_ = context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster();
}

std::string ViewingsPostHandler::ExtractToken(const userver::server::http::HttpRequest& request) const {
    auto auth_header = request.GetHeader("Authorization");
    if (auth_header.empty() || auth_header.find("Bearer ") != 0) {
        return "";
    }
    return auth_header.substr(7);
}

bool ViewingsPostHandler::ValidateToken(const std::string& token, std::string& username, int64_t& user_id) const {
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

userver::formats::json::Value ViewingsPostHandler::HandleRequestJsonThrow(
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
    
    if (!request_json.HasMember("property_id") || !request_json.HasMember("scheduled_time")) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Missing required fields: property_id, scheduled_time"});
    }
    
    int64_t property_id = request_json["property_id"].As<int64_t>();
    std::string scheduled_time = request_json["scheduled_time"].As<std::string>();
    std::string notes = request_json.HasMember("notes") ? 
                        request_json["notes"].As<std::string>() : "";
    
    // Проверяем существование объекта
    auto check_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT 1 FROM properties WHERE id = $1 LIMIT 1",
        property_id);
    
    if (check_result.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Property not found"});
    }
    
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO viewings (property_id, user_id, scheduled_time, notes, status) "
        "VALUES ($1, $2, $3::TIMESTAMP, $4, 'scheduled') RETURNING id, created_at",
        property_id, user_id, scheduled_time, notes);
    
    int64_t viewing_id = result[0][0].As<int64_t>();
    auto created_at = result[0][1].As<std::chrono::system_clock::time_point>();
    
    userver::formats::json::ValueBuilder response;
    response["id"] = viewing_id;
    response["property_id"] = property_id;
    response["user_id"] = user_id;
    response["scheduled_time"] = scheduled_time;
    response["status"] = "scheduled";
    if (!notes.empty()) {
        response["notes"] = notes;
    }
    response["created_at"] = std::to_string(std::chrono::system_clock::to_time_t(created_at));
    response["message"] = "Viewing scheduled successfully";
    
    return response.ExtractValue();
}

} // namespace pillow
