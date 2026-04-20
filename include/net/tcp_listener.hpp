#pragma once

#include "net/socket.hpp"
#include <string_view>
#include <cstdint>
#include <utility>

namespace net {
    class TcpListener {
    public:
        TcpListener(const std::string& address, uint16_t port, int backlog = 128, bool reuse_addr = true);
        ~TcpListener() = default;

        TcpListener(TcpListener&&) noexcept = default;
        TcpListener& operator=(TcpListener&&) noexcept = default;
        TcpListener(const TcpListener&) = delete;
        TcpListener& operator=(const TcpListener&) = delete;

        [[nodiscard]] std::pair<Socket, ClientAddress> accept();
        void close() noexcept;
        [[nodiscard]] bool is_open() const noexcept;
        [[nodiscard]] int native_handle() const noexcept;

    private:
        Socket socket;
    };
}
