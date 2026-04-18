#include "router/router.hpp"

namespace router {
    void Router::add_route(http::Method method, std::string_view path, Handler handler) {
        routes[make_key(method, path)] = std::move(handler);
    }

    http::Response Router::route(const http::Request& request) const {
        std::string key = make_key(request.method, request.path);
        auto it = routes.find(key);
        if (it != routes.end()) {
            return it->second(request);
        }

        std::string requested_path(request.path);
        std::string allowed_methods;

        for (const auto& [registered_key, _] : routes) {
            size_t space_pos = registered_key.find(' ');
            if (space_pos == std::string::npos) continue;

            std::string registered_path = registered_key.substr(space_pos + 1);
            if (registered_path == requested_path) {
                std::string method = registered_key.substr(0, space_pos);
                if (!allowed_methods.empty()) allowed_methods += ", ";
                allowed_methods += method;
            }
        }

        if (!allowed_methods.empty()) {
            return http::method_not_allowed(allowed_methods);
        }


        return http::not_found("Not Found");
    }

    std::string Router::make_key(http::Method method, std::string_view path) const noexcept {
        return method_to_string(method) + " " + std::string(path);
    }

    std::string Router::method_to_string(http::Method method) const noexcept {
        switch (method) {
            case http::Method::GET:
                return "GET";
            case http::Method::POST:
                return "POST";
            case http::Method::PUT:
                return "PUT";
            case http::Method::DELETE:
                return "DELETE";
            case http::Method::PATCH:
                return "PATCH";
            case http::Method::HEAD:
                return "HEAD";
            case http::Method::OPTIONS:
                return "OPTIONS";
            default:
                return "UNKNOWN";
        }
    }
}
