#pragma once

#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace pillow {

class ViewingsDeleteHandler final : public userver::server::handlers::HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "handler-viewings-delete";

    ViewingsDeleteHandler(const userver::components::ComponentConfig& config,
                          const userver::components::ComponentContext& context);

    userver::formats::json::Value HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_json,
        userver::server::request::RequestContext& context) const override;

private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
    
    std::string ExtractToken(const userver::server::http::HttpRequest& request) const;
    bool ValidateToken(const std::string& token, std::string& username, int64_t& user_id) const;
};

} // namespace pillow
