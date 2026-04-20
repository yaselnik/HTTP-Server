cmake_minimum_required(VERSION 3.20)
project(http-server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(net_lib STATIC
    src/net/socket.cpp
    src/net/tcp_listener.cpp
    src/net/io.cpp
)
target_include_directories(net_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(net_lib PRIVATE
    -Wall -Wextra -Wpedantic -Wconversion)

add_library(http_lib STATIC
    src/http/serializer.cpp
    src/http/response.cpp
    src/http/parser.cpp
)

target_include_directories(http_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(http_lib PUBLIC net_lib)

add_library(router_lib STATIC
    src/router/router.cpp
)

target_include_directories(router_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(router_lib PUBLIC http_lib)

add_library(server_lib STATIC
    src/server/thread_pool.cpp
    src/server/server.cpp
)

target_include_directories(server_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(server_lib PUBLIC router_lib)

add_executable(server_app src/main.cpp)

target_link_libraries(server_app PRIVATE server_lib)
