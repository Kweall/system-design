#include "handlers/auth/register/handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <regex>

namespace pillow {

static bool IsValidEmail(const std::string& email) {
    const std::regex pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w{2,})+)");
    return std::regex_match(email, pattern);
}

AuthRegisterHandler::AuthRegisterHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context) {
    
    pg_cluster_ = context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster();
}

userver::formats::json::Value AuthRegisterHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest&,
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext&) const {
    
    if (!request_json.HasMember("username") || !request_json.HasMember("password") || !request_json.HasMember("email")) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Missing required fields: username, password, email"});
    }
    
    std::string username = request_json["username"].As<std::string>();
    std::string password = request_json["password"].As<std::string>();
    std::string email = request_json["email"].As<std::string>();
    
    if (username.length() < 3 || username.length() > 50) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Username must be between 3 and 50 characters"});
    }
    
    if (password.length() < 6) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Password must be at least 6 characters"});
    }
    
    if (!IsValidEmail(email)) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Invalid email format"});
    }
    
    auto check_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT 1 FROM users WHERE username = $1 LIMIT 1",
        username);
    
    if (!check_result.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"User already exists"});
    }
    
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO users (username, password_hash, email) VALUES ($1, $2, $3) RETURNING id",
        username, password, email);
    
    int64_t user_id = result[0][0].As<int64_t>();
    
    userver::formats::json::ValueBuilder response;
    response["id"] = user_id;
    response["username"] = username;
    response["email"] = email;
    response["message"] = "User registered successfully";
    
    return response.ExtractValue();
}

} // namespace pillow
