#pragma once

#include <string>
#include <vector>
#include <utility>

namespace http {
    struct Response {
        int status_code = 200;
        std::string message = "OK";
        std::vector<std::pair<std::string, std::string>> headers;
        std::string body;
    };
    [[nodiscard]] Response ok(std::string body, std::string content_type);
    [[nodiscard]] Response created(std::string body = "");
    [[nodiscard]] Response bad_request(std::string error = "Bad Request");
    [[nodiscard]] Response not_found(std::string error = "Not Found");
    [[nodiscard]] Response internal_error(std::string error = "Internal Server Error");
    [[nodiscard]] Response method_not_allowed(std::string allowed_methods = "");
}
