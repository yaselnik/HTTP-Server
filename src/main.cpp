#include "server/server.hpp"
#include <iostream>

int main() {
    server::HttpServer server("0.0.0.0", 8080);

    server.add_route(http::Method::GET, "/ping",
        [](const http::Request&) {
            return http::ok("pong", "text/plain");
        });

    server.add_route(http::Method::GET, "/hello",
        [](const http::Request&) {
            return http::ok("<h1>Hello, World!</h1>", "text/html");
        });

    server.add_route(http::Method::POST, "/echo",
        [](const http::Request& req) {
            return http::ok(std::string(req.body), "text/plain");
        });

    server.add_route(http::Method::GET, "/json",
        [](const http::Request&) {
            return http::ok(R"({"status":"ok","message":"Hello from C++"})", "application/json");
        });

    server.add_route(http::Method::OPTIONS, "/ping",
        [](const http::Request&) {
            return http::method_not_allowed("GET, OPTIONS");
        });

    std::cout << "[INFO] Server started on http://0.0.0.0:8080\n";
    std::cout << "[INFO] Available routes: /ping, /hello, /echo (POST), /json\n";

    server.start();

    return 0;
}
