#include "handlers/properties/get_list/handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/storages/postgres/io/decimal64.hpp>
#include <userver/server/handlers/exceptions.hpp>

namespace pillow {

PropertiesGetListHandler::PropertiesGetListHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context) {
    
    pg_cluster_ = context.FindComponent<userver::components::Postgres>("postgres-db")
                      .GetCluster();
}

userver::formats::json::Value PropertiesGetListHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value&,
    userver::server::request::RequestContext&) const {
    
    try {
        std::string city = request.GetArg("city");
        std::string min_price_str = request.GetArg("min_price");
        std::string max_price_str = request.GetArg("max_price");
        
        std::string query = "SELECT id, title, description, price::double precision, city, rooms, owner_id, created_at FROM properties WHERE 1=1";
        
        if (!city.empty()) {
            query += " AND city = '" + city + "'";
        }
        if (!min_price_str.empty()) {
            query += " AND price >= " + min_price_str;
        }
        if (!max_price_str.empty()) {
            query += " AND price <= " + max_price_str;
        }
        
        query += " ORDER BY price";
        
        auto result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kSlave,
            query);
        
        userver::formats::json::ValueBuilder items;
        items = userver::formats::json::Type::kArray;
        
        for (const auto& row : result) {
            userver::formats::json::ValueBuilder item;
            item["id"] = row["id"].As<int64_t>();
            item["title"] = row["title"].As<std::string>();
            item["description"] = row["description"].As<std::string>();
            item["price"] = row["price"].As<double>();
            item["city"] = row["city"].As<std::string>();
            item["rooms"] = row["rooms"].As<int>();
            item["owner_id"] = row["owner_id"].As<int64_t>();
            
            auto created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
            item["created_at"] = std::to_string(std::chrono::system_clock::to_time_t(created_at));
            
            items.PushBack(item.ExtractValue());
        }
        
        return items.ExtractValue();
        
    } catch (const std::exception& e) {
        throw userver::server::handlers::InternalServerError(
            userver::server::handlers::ExternalBody{std::string("Database error: ") + e.what()});
    }
}

} // namespace pillow
