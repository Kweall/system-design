#include "handlers/properties/get/handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/chrono.hpp>

namespace pillow {

PropertiesGetHandler::PropertiesGetHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context) {
    
    pg_cluster_ = context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster();
}

userver::formats::json::Value PropertiesGetHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value&,
    userver::server::request::RequestContext&) const {
    
    std::string id_str = request.GetPathArg("id");
    if (id_str.empty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Property ID required"});
    }
    
    int64_t id = std::stoll(id_str);
    
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, title, description, price, city, rooms, owner_id, created_at FROM properties WHERE id = $1",
        id);
    
    if (result.IsEmpty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Property not found"});
    }
    
    userver::formats::json::ValueBuilder response;
    response["id"] = result[0]["id"].As<int64_t>();
    response["title"] = result[0]["title"].As<std::string>();
    response["description"] = result[0]["description"].As<std::string>();
    response["price"] = result[0]["price"].As<double>();
    response["city"] = result[0]["city"].As<std::string>();
    response["rooms"] = result[0]["rooms"].As<int>();
    response["owner_id"] = result[0]["owner_id"].As<int64_t>();
    
    auto created_at = result[0]["created_at"].As<std::chrono::system_clock::time_point>();
    response["created_at"] = std::to_string(std::chrono::system_clock::to_time_t(created_at));
    
    return response.ExtractValue();
}

} // namespace pillow
