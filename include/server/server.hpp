#pragma once

#include "net/tcp_listener.hpp"
#include <memory>
#include "net/io.hpp"
#include "router/router.hpp"
#include "server/thread_pool.hpp"

namespace server {
    class HttpServer {
    public:
        explicit HttpServer(const std::string& address, uint16_t port);

        void add_route(http::Method method, std::string_view path, router::Handler handler);
        void start();
        void stop() noexcept;

    private:
        void handle_connection(net::Socket client, const net::ClientAddress& addr);
        void send_error(net::Socket& client, int status_code, std::string message);

        std::string address;
        u_int16_t port;
        net::TcpListener listener;
        router::Router router;
        bool running = false;

        static constexpr size_t BUFFER_SIZE = 8192;
        static constexpr size_t MAX_REQUEST_SIZE = 1024 * 1024;
    };
}
