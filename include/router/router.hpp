#pragma once

#include "http/request.hpp"
#include "http/response.hpp"
#include <functional>
#include <string_view>
#include <unordered_map>

namespace router {
    using Handler = std::function<http::Response(const http::Request&)>;

    class Router {
    public:
        void add_route(http::Method method, std::string_view path, Handler handler);

        [[nodiscard]] http::Response route(const http::Request& request) const;

    private:
        [[nodiscard]] std::string make_key(http::Method method, std::string_view path) const noexcept;
        [[nodiscard]] std::string method_to_string(http::Method method) const noexcept;
        
        std::unordered_map<std::string, Handler> routes;
    };
}
