#include "handlers/mongo/properties/get/handler.hpp"

#include <userver/components/component_context.hpp>
#include <userver/formats/bson/inline.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/storages/mongo/collection.hpp>

#include <userver/utils/datetime.hpp>

namespace pillow {

MongoPropertiesGetHandler::MongoPropertiesGetHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : userver::server::handlers::HttpHandlerBase(config, context),
      mongo_pool_(context.FindComponent<userver::components::Mongo>("mongo-db").GetPool()) {}

std::string MongoPropertiesGetHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

    const std::string id_str = request.GetPathArg("id");
    if (id_str.empty()) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Property ID required"});
    }

    const int property_id = std::stoi(id_str);

    auto collection = mongo_pool_->GetCollection("properties");

    using userver::formats::bson::MakeDoc;
    auto doc = collection.FindOne(MakeDoc("property_id", property_id));

    if (!doc) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{"Property not found"});
    }

    const auto& bson = *doc;
    userver::formats::json::ValueBuilder vb;

    if (bson.HasMember("property_id")) {
        vb["property_id"] = bson["property_id"].As<int>();
    }

    if (bson.HasMember("title")) {
        vb["title"] = bson["title"].As<std::string>();
    }

    if (bson.HasMember("description")) {
        vb["description"] = bson["description"].As<std::string>();
    }

    if (bson.HasMember("price")) {
        vb["price"] = bson["price"].As<double>();
    }

    if (bson.HasMember("city")) {
        vb["city"] = bson["city"].As<std::string>();
    }

    if (bson.HasMember("rooms")) {
        vb["rooms"] = bson["rooms"].As<int>();
    }

    if (bson.HasMember("owner")) {
        const auto owner = bson["owner"];

        userver::formats::json::ValueBuilder owner_json;

        if (owner.HasMember("user_id")) {
            owner_json["user_id"] = owner["user_id"].As<int>();
        }

        if (owner.HasMember("username")) {
            owner_json["username"] = owner["username"].As<std::string>();
        }

        vb["owner"] = owner_json.ExtractValue();
    }

    if (bson.HasMember("viewings")) {
        const auto viewings = bson["viewings"];

        userver::formats::json::ValueBuilder arr(
            userver::formats::json::Type::kArray);

        for (const auto& item : viewings) {
            userver::formats::json::ValueBuilder v;

            if (item.HasMember("viewing_id")) {
                v["viewing_id"] = item["viewing_id"].As<int>();
            }

            if (item.HasMember("user_id")) {
                v["user_id"] = item["user_id"].As<int>();
            }

            if (item.HasMember("status")) {
                v["status"] = item["status"].As<std::string>();
            }

            if (item.HasMember("notes")) {
                v["notes"] = item["notes"].As<std::string>();
            }

            if (item.HasMember("scheduled_time")) {
                v["scheduled_time"] =
                    userver::utils::datetime::Timestring(
                        item["scheduled_time"]
                            .As<std::chrono::system_clock::time_point>());
            }

            arr.PushBack(v.ExtractValue());
        }

        vb["viewings"] = arr.ExtractValue();
    }

    if (bson.HasMember("created_at")) {
        vb["created_at"] =
            userver::utils::datetime::Timestring(
                bson["created_at"]
                    .As<std::chrono::system_clock::time_point>());
    }

    if (bson.HasMember("updated_at")) {
        vb["updated_at"] =
            userver::utils::datetime::Timestring(
                bson["updated_at"]
                    .As<std::chrono::system_clock::time_point>());
    }

    if (bson.HasMember("_id")) {
        vb["_id"] = bson["_id"]
            .As<userver::formats::bson::Oid>()
            .ToString();
    }

    return userver::formats::json::ToString(vb.ExtractValue());
}

}