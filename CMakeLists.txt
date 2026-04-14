cmake_minimum_required(VERSION 3.20)
project(http-server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(net_lib
    src/net/socket.cpp
    src/net/tcp_listener.cpp
)
target_include_directories(net_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(net_lib PRIVATE
    -Wall -Wextra -Wpedantic -Wconversion)

add_executable(server_app src/main.cpp)

target_link_libraries(server_app PRIVATE net_lib)
