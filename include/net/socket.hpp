#pragma once

#include <sys/socket.h>
#include <utility>


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
        void bind(const sockaddr* addr, socklen_t addrlen) const;
        void listen(int backlog) const;
        [[nodiscard]] std::pair<Socket, ClientAddress> accept() const;
    };
}
