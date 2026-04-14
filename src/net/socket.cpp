#include "net/socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <utility>
#include <string>
#include <system_error>

namespace {
    void check_error(int result, const std::string& context) {
        if (result == -1) {
            throw std::system_error(errno, std::system_category(), context);
        }
    }
}

namespace net {
    Socket::Socket(int domain, int type, int protocol) {
        fd = ::socket(domain, type, protocol);
        check_error(fd, "socket()");
    }

    Socket::Socket(int fd) noexcept
    : fd(fd) {}

    Socket::~Socket() {
        close();
    }

    Socket::Socket(Socket&& other) noexcept
        : fd(std::exchange(other.fd, -1)) {}

    Socket& Socket::operator=(Socket&& other) noexcept {
        if (this != &other) {
            close();
            fd = std::exchange(other.fd, -1);
        }
        return *this;
    }

    void Socket::close() noexcept {
        if (fd != -1) {
            ::close(fd);
            fd = -1;
        }
    }

    void Socket::bind(const sockaddr* addr, socklen_t addrlen) const {
        check_error(::bind(fd, addr, addrlen), "bind()");
    }

    void Socket::listen(int backlog) const {
        check_error(::listen(fd, backlog), "listen()");
    }

    std::pair<Socket, ClientAddress> Socket::accept() const {
        sockaddr_storage addr {0};
        socklen_t addr_len = sizeof(addr);

        int client_fd;
        do {
            client_fd = ::accept(fd, reinterpret_cast<sockaddr*>(&addr), &addr_len);
        } while (client_fd == -1 && errno == EINTR);

        check_error(client_fd, "accept()");


        char ip[INET6_ADDRSTRLEN];
        uint16_t port = 0;

        if (addr.ss_family == AF_INET) {
            const auto* a4 = (sockaddr_in*)&addr;
            inet_ntop(AF_INET, &a4->sin_addr, ip, sizeof(ip));
            port = ntohs(a4->sin_port);
        } else if (addr.ss_family == AF_INET6) {
            const auto* a6 = (sockaddr_in6*)&addr;
            inet_ntop(AF_INET6, &a6->sin6_addr, ip, sizeof(ip));
            port = ntohs(a6->sin6_port);
        } else {
            return {Socket(client_fd), ClientAddress{"unknown", 0}};
        }

        return {Socket(client_fd), ClientAddress{ip, port}};
    }
}
