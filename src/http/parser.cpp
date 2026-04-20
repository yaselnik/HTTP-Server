#include "http/parser.hpp"
#include <string_view>
#include <vector>
#include <algorithm>
#include <cctype>
#include <charconv>
#include <variant>

namespace {
    std::string_view trim(std::string_view sv) noexcept {
        auto start = sv.find_first_not_of(" \t");
        if (start == std::string_view::npos) return "";
        auto end = sv.find_last_not_of(" \t");
        return sv.substr(start, end - start + 1);
    }

    bool comp_header(std::string_view str, std::string_view prefix) noexcept {
        if (str.size() < prefix.size()) return false;
        for (size_t i = 0; i < prefix.size(); ++i) {
            if (std::tolower(static_cast<unsigned char>(str[i])) !=
                std::tolower(static_cast<unsigned char>(prefix[i]))) {
                return false;
            }
        }
        return str.size() == prefix.size();
    }

    http::Method parseMethod(std::string_view method_str) {
        if (method_str == "GET")     return http::Method::GET;
        if (method_str == "POST")    return http::Method::POST;
        if (method_str == "PUT")     return http::Method::PUT;
        if (method_str == "DELETE")  return http::Method::DELETE;
        if (method_str == "HEAD")    return http::Method::HEAD;
        if (method_str == "PATCH")   return http::Method::PATCH;
        if (method_str == "OPTIONS") return http::Method::OPTIONS;
        return http::Method::UNKNOWN;
    }

    std::variant<http::ParseError, size_t> parseHeaders(std::string_view headers_block,
                                                        size_t pos,
                                                        std::vector<std::pair<std::string_view, std::string_view>>& headers) {
        size_t header_count = 0;
        size_t content_length = 0;
        bool has_content_length = false;

        while (pos < headers_block.size()) {
            size_t header_end = headers_block.find("\r\n", pos);
            if (header_end == std::string_view::npos) {
                header_end = headers_block.size();
            }

            size_t header_len = header_end - pos;
            if (header_len > http::MAX_HEADER_LEN) {
                return http::ParseError::HEADER_TOO_LONG;
            }

            if (++header_count > http::MAX_HEADERS) {
                return http::ParseError::HEADER_TOO_LONG;
            }

            std::string_view header = headers_block.substr(pos, header_end - pos);
            pos = header_end + 2;

            if (header.empty()) return http::ParseError::BAD_FORMAT;

            size_t colon = header.find(':');
            if (colon == std::string_view::npos)
                return http::ParseError::BAD_FORMAT;

            std::string_view key = trim(header.substr(0, colon));
            std::string_view val = trim(header.substr(colon + 1));

            if (key.empty()) return http::ParseError::BAD_FORMAT;
            headers.push_back(std::make_pair(key, val));

            if (comp_header(key, "content-length")) {
                if (has_content_length)
                    return http::ParseError::BAD_FORMAT;

                auto res = std::from_chars(val.data(), val.data() + val.size(), content_length);
                if (res.ec != std::errc() || res.ptr != val.data() + val.size())
                    return http::ParseError::BAD_FORMAT;
                has_content_length = true;
            }
        }

        return content_length;
    }
}

namespace http {
    ParseResult parse(std::string_view data,
                      Request& request) noexcept {
        request.headers.clear();
        request.method = Method::UNKNOWN;
        request.body = {};
        request.path = {};
        request.version = {};

        auto header_sep = data.find("\r\n\r\n");
        if (header_sep == std::string_view::npos) {
            return {ParseError::INCOMPLETE, 0};
        }
        auto headers_block = data.substr(0, header_sep);
        auto body_block = data.substr(header_sep + 4);

        size_t first_crlf = headers_block.find("\r\n");
        if (first_crlf == std::string_view::npos)
            return {ParseError::BAD_FORMAT, 0};

        std::string_view start_line = headers_block.substr(0, first_crlf);

        size_t sp1 = start_line.find(' ');
        if (sp1 == std::string_view::npos) return {ParseError::BAD_FORMAT, 0};

        size_t sp2 = start_line.find(' ', sp1 + 1);
        if (sp2 == std::string_view::npos) return {ParseError::BAD_FORMAT, 0};

        std::string_view method_str = start_line.substr(0, sp1);
        std::string_view path       = start_line.substr(sp1 + 1, sp2 - sp1 - 1);
        std::string_view version    = start_line.substr(sp2 + 1);

        request.method = parseMethod(method_str);
        if (request.method == Method::UNKNOWN) return {ParseError::INVALID_METHOD, 0};

        if (version != "HTTP/1.0" && version != "HTTP/1.1")
            return {ParseError::INVALID_VERSION, 0};

        if (path.empty()) return {ParseError::BAD_FORMAT, 0};

        request.path    = path;
        request.version = version;

        size_t pos = first_crlf + 2;
        auto res = parseHeaders(headers_block, pos, request.headers);

        if (std::holds_alternative<ParseError>(res))
            return {std::get<ParseError>(res), 0};

        auto content_length = std::get<size_t>(res);

        if (content_length > 0) {
            if (content_length > MAX_BODY_SIZE)
                return {ParseError::BODY_TOO_LARGE, 0};

            if (body_block.size() < content_length)
                return {ParseError::INCOMPLETE, 0};

            request.body = body_block.substr(0, content_length);
            return {ParseError::NONE, header_sep + 4 + content_length};
        }

        request.body = "";
        return {ParseError::NONE, header_sep + 4};
    }
}
