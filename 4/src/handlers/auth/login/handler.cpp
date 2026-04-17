#include "handlers/auth/login/handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>

namespace pillow {

AuthLoginHandler::AuthLoginHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context) {
    
    pg_cluster_ = context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster();
}

userver::formats::json::Value AuthLoginHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest&,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext&) const {
    
    if (!request_json.HasMember("username") || !request_json.HasMember("password")) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Missing required fields: username, password"});
    }
    
    std::string username = request_json["username"].As<std::string>();
    std::string password = request_json["password"].As<std::string>();
    
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id FROM users WHERE username = $1 AND password_hash = $2",
        username, password);
    
    if (result.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Invalid credentials"});
    }
    
    int64_t user_id = result[0][0].As<int64_t>();
    std::string token = "jwt_" + username + "_" + std::to_string(std::time(nullptr));
    
    userver::formats::json::ValueBuilder response;
    response["access_token"] = token;
    response["token_type"] = "Bearer";
    response["expires_in"] = 3600;
    response["user"]["id"] = user_id;
    response["user"]["username"] = username;
    
    return response.ExtractValue();
}

} // namespace pillow
