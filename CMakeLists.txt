cmake_minimum_required(VERSION 3.14)
project(pillow-api CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(USERVER_FEATURE_POSTGRESQL ON)
set(USERVER_FEATURE_REDIS OFF)
set(USERVER_FEATURE_MONGOOSE OFF)
set(USERVER_FEATURE_CLICKHOUSE OFF)
set(USERVER_FEATURE_GRPC OFF)
set(USERVER_FEATURE_KAFKA OFF)
set(USERVER_FEATURE_RABBITMQ OFF)
set(USERVER_FEATURE_MYSQL OFF)
set(USERVER_FEATURE_SQLITE OFF)
set(USERVER_FEATURE_ODBC OFF)

find_package(userver REQUIRED COMPONENTS core postgresql)

set(SOURCES
    src/main.cpp
    src/handlers/auth/login/handler.cpp
    src/handlers/auth/register/handler.cpp
    src/handlers/properties/get_list/handler.cpp
    src/handlers/properties/get/handler.cpp
    src/handlers/properties/post/handler.cpp
    src/handlers/properties/put/handler.cpp
    src/handlers/properties/delete/handler.cpp
    src/handlers/viewings/get/handler.cpp
    src/handlers/viewings/post/handler.cpp
    src/handlers/viewings/put/handler.cpp
    src/handlers/viewings/delete/handler.cpp
    src/handlers/users/me/handler.cpp
)

add_executable(pillow-api ${SOURCES})

target_link_libraries(pillow-api userver::core userver::postgresql)
target_include_directories(pillow-api PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_options(pillow-api PRIVATE -Wall -Wextra -Wno-unused-parameter)