#pragma once

#include <string_view>
#include <vector>

namespace http {
    enum class Method {
        GET,
        POST,
        PUT,
        DELETE,
        PATCH,
        HEAD,
        OPTIONS,
        UNKNOWN,
    };

    struct Request {
        Method method = Method::UNKNOWN;
        std::string_view path;
        std::string_view version;
        std::vector<std::pair<std::string_view, std::string_view>> headers;
        std::string_view body;
    };

    enum class ParseError {
        NONE,
        INCOMPLETE,
        BAD_FORMAT,
        INVALID_METHOD,
        INVALID_VERSION,
        HEADER_TOO_LONG,
        BODY_TOO_LARGE,
        MISSING_CRLF
    };

    struct ParseResult {
        ParseError error = ParseError::NONE;
        size_t bytes_consumed = 0;
    };

}
