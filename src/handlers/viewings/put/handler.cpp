#include "handlers/viewings/put/handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>

namespace pillow {

ViewingsPutHandler::ViewingsPutHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context) {
    
    pg_cluster_ = context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster();
}

std::string ViewingsPutHandler::ExtractToken(const userver::server::http::HttpRequest& request) const {
    auto auth_header = request.GetHeader("Authorization");
    if (auth_header.empty() || auth_header.find("Bearer ") != 0) {
        return "";
    }
    return auth_header.substr(7);
}

bool ViewingsPutHandler::ValidateToken(const std::string& token, std::string& username, int64_t& user_id) const {
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

userver::formats::json::Value ViewingsPutHandler::HandleRequestJsonThrow(
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
            userver::server::handlers::ExternalBody{"Viewing ID required"});
    }
    
    int64_t viewing_id = std::stoll(id_str);
    
    // Проверяем существование и права
    auto check_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT user_id FROM viewings WHERE id = $1",
        viewing_id);
    
    if (check_result.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Viewing not found"});
    }
    
    int64_t owner_id = check_result[0][0].As<int64_t>();
    if (owner_id != user_id) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"You don't have permission to update this viewing"});
    }
    
    if (!request_json.HasMember("status")) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Missing required field: status"});
    }
    
    std::string status = request_json["status"].As<std::string>();
    if (status != "confirmed" && status != "cancelled" && status != "completed") {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Invalid status. Allowed: confirmed, cancelled, completed"});
    }
    
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE viewings SET status = $1, updated_at = CURRENT_TIMESTAMP WHERE id = $2 RETURNING id",
        status, viewing_id);
    
    if (result.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Failed to update viewing"});
    }
    
    userver::formats::json::ValueBuilder response;
    response["id"] = viewing_id;
    response["status"] = status;
    response["message"] = "Viewing updated successfully";
    
    return response.ExtractValue();
}

} // namespace pillow
