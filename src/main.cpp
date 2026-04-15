#include "http/response.hpp"
#include "http/serializer.hpp"
#include <iostream>
#include <cassert>
#include <string_view>

void check_starts_with(std::string_view str, std::string_view prefix, const char* test_name) {
    if (str.substr(0, prefix.size()) != prefix) {
        std::cerr << "[FAIL] " << test_name << ": expected prefix '" << prefix
                  << "', got '" << str.substr(0, 30) << "'\n";
        std::exit(1);
    }
    std::cout << "[PASS] " << test_name << "\n";
}

void check_contains(std::string_view str, std::string_view substr, const char* test_name) {
    if (str.find(substr) == std::string_view::npos) {
        std::cerr << "[FAIL] " << test_name << ": expected to contain '" << substr << "'\n";
        std::exit(1);
    }
    std::cout << "[PASS] " << test_name << "\n";
}

void check_equals(std::string_view actual, std::string_view expected, const char* test_name) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << test_name << ": expected '" << expected
                  << "', got '" << actual << "'\n";
        std::exit(1);
    }
    std::cout << "[PASS] " << test_name << "\n";
}

void check_content_length(std::string_view serialized, const std::string& body, const char* test_name) {
    auto pos = serialized.find("Content-Length: ");
    if (pos == std::string_view::npos) {
        std::cerr << "[FAIL] " << test_name << ": missing Content-Length header\n";
        std::exit(1);
    }

    auto end = serialized.find("\r\n", pos);
    auto value_str = serialized.substr(pos + 16, end - pos - 16);
    size_t declared_length = std::stoul(std::string(value_str));

    if (declared_length != body.size()) {
        std::cerr << "[FAIL] " << test_name << ": Content-Length mismatch (declared: "
                  << declared_length << ", actual: " << body.size() << ")\n";
        std::exit(1);
    }
    std::cout << "[PASS] " << test_name << "\n";
}

int main() {
    std::cout << "=== Testing HTTP Response & Serializer ===\n\n";

    {
        auto resp = http::ok("pong", "text/plain");
        auto serialized = http::serialize(resp);

        check_starts_with(serialized, "HTTP/1.1 200 OK\r\n", "ok() status line");
        check_contains(serialized, "Content-Type: text/plain\r\n", "ok() Content-Type header");
        check_content_length(serialized, "pong", "ok() Content-Length");
        check_contains(serialized, "\r\n\r\npong", "ok() body separator");
    }

    {
        auto resp = http::not_found("Page not found");
        auto serialized = http::serialize(resp);

        check_starts_with(serialized, "HTTP/1.1 404 Not Found\r\n", "not_found() status line");
        check_content_length(serialized, "Page not found", "not_found() Content-Length");
    }

    {
        auto resp = http::bad_request("Invalid JSON");
        auto serialized = http::serialize(resp);

        check_starts_with(serialized, "HTTP/1.1 400 Bad Request\r\n", "bad_request() status line");
        check_content_length(serialized, "Invalid JSON", "bad_request() Content-Length");
    }

    {
        auto resp = http::method_not_allowed("GET");
        auto serialized = http::serialize(resp);

        check_starts_with(serialized, "HTTP/1.1 405 Method Not Allowed\r\n", "method_not_allowed() status line");
        check_contains(serialized, "Allow: ", "method_not_allowed() Allow header");
    }

    {
        auto resp = http::created("{\"id\": 1}");
        auto serialized = http::serialize(resp);

        check_starts_with(serialized, "HTTP/1.1 201 Created\r\n", "created() status line");
        check_content_length(serialized, "{\"id\": 1}", "created() Content-Length");
    }

    {
        auto resp = http::internal_error("Database connection failed");
        auto serialized = http::serialize(resp);

        check_starts_with(serialized, "HTTP/1.1 500 Internal Server Error\r\n", "internal_error() status line");
        check_content_length(serialized, "Database connection failed", "internal_error() Content-Length");
    }

    {
        auto resp = http::ok("", "text/plain");
        auto serialized = http::serialize(resp);

        check_content_length(serialized, "", "empty body Content-Length");
        check_contains(serialized, "\r\n\r\n", "empty body separator");
    }

    {
        http::Response resp;
        resp.status_code = 200;
        resp.message = "OK";
        resp.body = "test";
        resp.headers.push_back({"X-Custom-Header", "custom-value"});
        resp.headers.push_back({"Cache-Control", "no-cache"});

        auto serialized = http::serialize(resp);

        check_contains(serialized, "X-Custom-Header: custom-value\r\n", "custom header 1");
        check_contains(serialized, "Cache-Control: no-cache\r\n", "custom header 2");
    }

    {
        auto resp = http::ok("test", "text/plain");
        auto serialized = http::serialize(resp);

        if (serialized.find("\r\n") == std::string_view::npos) {
            std::cerr << "[FAIL] CRLF validation: no \\r\\n found\n";
            std::exit(1);
        }

        size_t pos = 0;
        while ((pos = serialized.find('\n', pos)) != std::string_view::npos) {
            if (pos == 0 || serialized[pos - 1] != '\r') {
                std::cerr << "[FAIL] CRLF validation: \\n without \\r at position " << pos << "\n";
                std::exit(1);
            }
            pos++;
        }
        std::cout << "[PASS] CRLF format validation\n";
    }

    {
        std::cout << "\n=== Sample Raw HTTP Response ===\n";
        auto resp = http::ok("Hello, World!", "text/html");
        resp.headers.push_back({"X-Server", "CppHTTP/1.0"});
        auto serialized = http::serialize(resp);
        std::cout << serialized << "\n";
        std::cout << "=== End Sample ===\n\n";
    }

    std::cout << "=== All tests passed! ===\n";
    return 0;
}
