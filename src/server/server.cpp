#include "server/server.hpp"
#include <iostream>
#include <cstring>
#include <memory>
#include "http/parser.hpp"
#include "http/serializer.hpp"
#include "server/thread_pool.hpp"

namespace server {
    HttpServer::HttpServer(const std::string& address, uint16_t port)
        :
        address(address),
        port(port),
        listener(address, port) {}

    void HttpServer::add_route(http::Method method, std::string_view path, router::Handler handler) {
        router.add_route(method, path, handler);
    }

    void HttpServer::start() {
        ThreadPool pool;
        running = true;

        while (running) {
            auto [client, addr] = listener.accept();
            pool.submit([this, client = std::move(client), addr]() mutable{
                handle_connection(std::move(client), addr);
            });
        }
    }

    void HttpServer::stop() noexcept {
        running = false;
        listener.close();
    }

    void HttpServer::handle_connection(net::Socket client, const net::ClientAddress& addr) {
        std::vector<char> buffer(BUFFER_SIZE);
        size_t buffer_size = 0;

        while (running && client.is_valid()) {
            if (buffer_size < BUFFER_SIZE) {
                size_t n = net::read_some(
                    client,
                    std::span{buffer.data() + buffer_size, buffer.size() - buffer_size}
                );
                if (n == 0) break;
                buffer_size += n;
            }

            http::Request request;
            auto result = http::parse(std::string_view{buffer.data(), buffer_size}, request);
            if (result.error == http::ParseError::INCOMPLETE) {
                if (buffer_size >= MAX_REQUEST_SIZE) {
                    send_error(client, 431, "Request Header Fields Too Large");
                    break;
                }
                continue;
            }

            if (result.error == http::ParseError::HEADER_TOO_LONG) {
                send_error(client, 431, "Request Header Fields Too Large");
                break;
            }

            if (result.error == http::ParseError::BODY_TOO_LARGE) {
                send_error(client, 413, "Payload Too Large");
                break;
            }

            if (result.error != http::ParseError::NONE) {
                send_error(client, 400, "Bad Request");
                break;
            }

            http::Response response;
            try {
                response = router.route(request);
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Handler exception: " << e.what() << "\n";
                response = http::Response{500, "Internal Server Error", {}, "Handler error"};
            }

            auto raw = http::serialize(response);
            net::write_all(client, std::span{raw});

            size_t consumed = result.bytes_consumed;
            size_t remaining = buffer_size - consumed;
            if (remaining > 0 && consumed > 0) {
                std::memmove(buffer.data(), buffer.data() + consumed, remaining);
            }
            buffer_size = remaining;

            bool should_keep_alive = (request.version == "HTTP/1.1");
            for (const auto& [key, val] : request.headers) {
                if (key == "Connection" || key == "connection") {
                    should_keep_alive = val == "keep-alive";
                    break;
                }
            }
            if (!should_keep_alive) break;
        }
    }

    void HttpServer::send_error(net::Socket& client, int status_code, std::string message) {
        http::Response resp;
        resp.status_code = status_code;
        resp.message = std::move(message);
        resp.body = "<h1>" + std::to_string(status_code) + " " + message + "</h1>";
        resp.headers.push_back({"Content-Type", "text/html"});
        auto raw = http::serialize(resp);
        net::write_all(client, std::span{raw});
    }
}
