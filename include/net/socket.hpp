#pragma once

#include <sys/socket.h>
#include <utility>
#include <string>
#include <cinttypes>

namespace net {
    struct ClientAddress {
        std::string ip;
        uint16_t port;
    };

    class Socket {
    private:
        int fd = -1;
    public:
        Socket() noexcept = default;
        explicit Socket(int domain, int type, int protocol = 0);
        explicit Socket(int fd) noexcept;
        ~Socket();

        Socket(Socket&& other) noexcept;
        Socket& operator=(Socket&& other) noexcept;
        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;

        void close() noexcept;
        void bind(const sockaddr* addr, socklen_t addrlen);
        void listen(int backlog);
        [[nodiscard]] std::pair<Socket, ClientAddress> accept() const;

        bool is_valid() const noexcept;
        int native_handle() const noexcept;
        void set_reuse_address(bool enable);
    };
}
