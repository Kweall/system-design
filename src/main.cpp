#include <userver/components/minimal_server_component_list.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/server/handlers/ping.hpp>

#include "handlers/auth/login/handler.hpp"
#include "handlers/auth/register/handler.hpp"
#include "handlers/properties/get_list/handler.hpp"
#include "handlers/properties/get/handler.hpp"
#include "handlers/properties/post/handler.hpp"
#include "handlers/properties/put/handler.hpp"
#include "handlers/properties/delete/handler.hpp"
#include "handlers/viewings/get/handler.hpp"
#include "handlers/viewings/post/handler.hpp"
#include "handlers/viewings/put/handler.hpp"
#include "handlers/viewings/delete/handler.hpp"
#include "handlers/users/me/handler.hpp"

int main(int argc, char* argv[]) {
    auto component_list = userver::components::MinimalServerComponentList()
        .Append<userver::components::TestsuiteSupport>()
        .Append<userver::clients::dns::Component>()
        .Append<userver::components::Postgres>("postgres-db")
        .Append<userver::server::handlers::Ping>("handler-ping")
        .Append<pillow::AuthLoginHandler>("auth-login-handler")
        .Append<pillow::AuthRegisterHandler>("auth-register-handler")
        .Append<pillow::PropertiesGetListHandler>("properties-get-list-handler")
        .Append<pillow::PropertiesGetHandler>("properties-get-handler")
        .Append<pillow::PropertiesPostHandler>("properties-post-handler")
        .Append<pillow::PropertiesPutHandler>("properties-put-handler")
        .Append<pillow::PropertiesDeleteHandler>("properties-delete-handler")
        .Append<pillow::ViewingsGetHandler>("viewings-get-handler")
        .Append<pillow::ViewingsPostHandler>("viewings-post-handler")
        .Append<pillow::ViewingsPutHandler>("viewings-put-handler")
        .Append<pillow::ViewingsDeleteHandler>("viewings-delete-handler")
        .Append<pillow::UsersMeHandler>("users-me-handler");

    return userver::utils::DaemonMain(argc, argv, component_list);
}
