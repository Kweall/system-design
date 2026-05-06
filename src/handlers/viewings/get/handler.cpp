#include "handlers/viewings/get/handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace pillow {

static std::string FormatTimePoint(const std::chrono::system_clock::time_point& tp) {
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm tm;
    gmtime_r(&tt, &tm);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

ViewingsGetHandler::ViewingsGetHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context) {
    
    pg_cluster_ = context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster();
}

std::string ViewingsGetHandler::ExtractToken(const userver::server::http::HttpRequest& request) const {
    auto auth_header = request.GetHeader("Authorization");
    if (auth_header.empty() || auth_header.find("Bearer ") != 0) {
        return "";
    }
    return auth_header.substr(7);
}

bool ViewingsGetHandler::ValidateToken(const std::string& token, std::string& username, int64_t& user_id) const {
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
        "SELECT id FROM users WHERE username = '" + username + "'");
    
    if (result.IsEmpty()) {
        return false;
    }
    
    user_id = result[0][0].As<int64_t>();
    return true;
}

userver::formats::json::Value ViewingsGetHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value&,
    userver::server::request::RequestContext&) const {
    
    try {
        std::string token = ExtractToken(request);
        std::string username;
        int64_t user_id;
        
        if (token.empty() || !ValidateToken(token, username, user_id)) {
            throw userver::server::handlers::ClientError(
                userver::server::handlers::ExternalBody{"Authentication required"});
        }
        
        std::string status = request.GetArg("status");
        
        std::string query = "SELECT id, property_id, user_id, scheduled_time, status, notes, created_at "
                            "FROM viewings WHERE user_id = " + std::to_string(user_id);
        
        if (!status.empty()) {
            query += " AND status = '" + status + "'";
        }
        
        query += " ORDER BY scheduled_time";
        
        auto result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kSlave,
            query);
        
        userver::formats::json::ValueBuilder items;
        items = userver::formats::json::Type::kArray;
        
        for (const auto& row : result) {
            userver::formats::json::ValueBuilder item;
            item["id"] = row["id"].As<int64_t>();
            item["property_id"] = row["property_id"].As<int64_t>();
            item["user_id"] = row["user_id"].As<int64_t>();
            
            auto scheduled_time = row["scheduled_time"].As<std::chrono::system_clock::time_point>();
            item["scheduled_time"] = FormatTimePoint(scheduled_time);
            
            item["status"] = row["status"].As<std::string>();
            
            auto notes = row["notes"].As<std::string>();
            if (!notes.empty()) {
                item["notes"] = notes;
            }
            
            auto created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
            item["created_at"] = FormatTimePoint(created_at);
            
            items.PushBack(item.ExtractValue());
        }
        
        return items.ExtractValue();
        
    } catch (const userver::server::handlers::ClientError&) {
        throw;
    } catch (const std::exception& e) {
        throw userver::server::handlers::InternalServerError(
            userver::server::handlers::ExternalBody{std::string("Database error: ") + e.what()});
    }
}

} // namespace pillow
