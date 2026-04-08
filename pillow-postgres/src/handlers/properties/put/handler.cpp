#include "handlers/properties/put/handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <boost/algorithm/string/join.hpp>

namespace pillow {

PropertiesPutHandler::PropertiesPutHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context) {
    
    pg_cluster_ = context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster();
}

std::string PropertiesPutHandler::ExtractToken(const userver::server::http::HttpRequest& request) const {
    auto auth_header = request.GetHeader("Authorization");
    if (auth_header.empty() || auth_header.find("Bearer ") != 0) {
        return "";
    }
    return auth_header.substr(7);
}

bool PropertiesPutHandler::ValidateToken(const std::string& token, std::string& username, int64_t& user_id) const {
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

userver::formats::json::Value PropertiesPutHandler::HandleRequestJsonThrow(
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
    
    std::string id_str = request.GetPathArg("id");
    if (id_str.empty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Property ID required"});
    }
    
    int64_t property_id = std::stoll(id_str);
    
    // Проверяем существование и права
    auto check_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT owner_id FROM properties WHERE id = $1",
        property_id);
    
    if (check_result.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Property not found"});
    }
    
    int64_t owner_id = check_result[0][0].As<int64_t>();
    if (owner_id != user_id) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"You don't have permission to update this property"});
    }
    
    // Обновляем поля
    std::vector<std::string> set_parts;
    std::vector<std::string> params;
    int param_counter = 1;
    
    if (request_json.HasMember("title")) {
        set_parts.push_back("title = $" + std::to_string(param_counter++));
        params.push_back(request_json["title"].As<std::string>());
    }
    if (request_json.HasMember("description")) {
        set_parts.push_back("description = $" + std::to_string(param_counter++));
        params.push_back(request_json["description"].As<std::string>());
    }
    if (request_json.HasMember("price")) {
        set_parts.push_back("price = $" + std::to_string(param_counter++));
        params.push_back(std::to_string(request_json["price"].As<double>()));
    }
    if (request_json.HasMember("city")) {
        set_parts.push_back("city = $" + std::to_string(param_counter++));
        params.push_back(request_json["city"].As<std::string>());
    }
    if (request_json.HasMember("rooms")) {
        set_parts.push_back("rooms = $" + std::to_string(param_counter++));
        params.push_back(std::to_string(request_json["rooms"].As<int>()));
    }
    
    set_parts.push_back("updated_at = CURRENT_TIMESTAMP");
    
    if (set_parts.empty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"No fields to update"});
    }
    
    std::string query = "UPDATE properties SET " + boost::algorithm::join(set_parts, ", ") + 
                        " WHERE id = $" + std::to_string(param_counter) + " RETURNING id";
    params.push_back(std::to_string(property_id));
    
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        query, params);
    
    if (result.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Failed to update property"});
    }
    
    // Получаем обновленные данные
    auto get_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, title, description, price, city, rooms, owner_id, created_at FROM properties WHERE id = $1",
        property_id);
    
    userver::formats::json::ValueBuilder response;
    response["id"] = get_result[0]["id"].As<int64_t>();
    response["title"] = get_result[0]["title"].As<std::string>();
    response["description"] = get_result[0]["description"].As<std::string>();
    response["price"] = get_result[0]["price"].As<double>();
    response["city"] = get_result[0]["city"].As<std::string>();
    response["rooms"] = get_result[0]["rooms"].As<int>();
    response["owner_id"] = get_result[0]["owner_id"].As<int64_t>();
    
    auto created_at = get_result[0]["created_at"].As<std::chrono::system_clock::time_point>();
    response["created_at"] = std::to_string(std::chrono::system_clock::to_time_t(created_at));
    response["message"] = "Property updated successfully";
    
    return response.ExtractValue();
}

} // namespace pillow
