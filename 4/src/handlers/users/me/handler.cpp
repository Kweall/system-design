#include "handlers/users/me/handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/chrono.hpp>

namespace pillow {

UsersMeHandler::UsersMeHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context) {
    
    pg_cluster_ = context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster();
}

std::string UsersMeHandler::ExtractToken(const userver::server::http::HttpRequest& request) const {
    auto auth_header = request.GetHeader("Authorization");
    if (auth_header.empty() || auth_header.find("Bearer ") != 0) {
        return "";
    }
    return auth_header.substr(7);
}

bool UsersMeHandler::ValidateToken(const std::string& token, std::string& username, int64_t& user_id) const {
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
        "SELECT id, username, email, created_at FROM users WHERE username = $1",
        username);
    
    if (result.IsEmpty()) {
        return false;
    }
    
    user_id = result[0][0].As<int64_t>();
    return true;
}

userver::formats::json::Value UsersMeHandler::HandleRequestJsonThrow(
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
    
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, username, email, created_at FROM users WHERE id = $1",
        user_id);
    
    if (result.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"User not found"});
    }
    
    userver::formats::json::ValueBuilder response;
    response["id"] = result[0]["id"].As<int64_t>();
    response["username"] = result[0]["username"].As<std::string>();
    response["email"] = result[0]["email"].As<std::string>();
    
    auto created_at = result[0]["created_at"].As<std::chrono::system_clock::time_point>();
    response["created_at"] = std::to_string(std::chrono::system_clock::to_time_t(created_at));
    
    return response.ExtractValue();
}

} // namespace pillow
