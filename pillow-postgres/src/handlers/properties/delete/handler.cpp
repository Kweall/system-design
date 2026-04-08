#include "handlers/properties/delete/handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>

namespace pillow {

PropertiesDeleteHandler::PropertiesDeleteHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context) {
    
    pg_cluster_ = context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster();
}

std::string PropertiesDeleteHandler::ExtractToken(const userver::server::http::HttpRequest& request) const {
    auto auth_header = request.GetHeader("Authorization");
    if (auth_header.empty() || auth_header.find("Bearer ") != 0) {
        return "";
    }
    return auth_header.substr(7);
}

bool PropertiesDeleteHandler::ValidateToken(const std::string& token, std::string& username, int64_t& user_id) const {
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

userver::formats::json::Value PropertiesDeleteHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value&,
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
            userver::server::handlers::ExternalBody{"You don't have permission to delete this property"});
    }
    
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "DELETE FROM properties WHERE id = $1 RETURNING id",
        property_id);
    
    if (result.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Failed to delete property"});
    }
    
    userver::formats::json::ValueBuilder response;
    response["message"] = "Property deleted successfully";
    response["id"] = property_id;
    
    return response.ExtractValue();
}

} // namespace pillow
