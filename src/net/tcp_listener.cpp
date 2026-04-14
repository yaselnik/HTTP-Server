#include "net/tcp_listener.hpp"
#include <netdb.h>
#include <stdexcept>
#include <memory>
#include <string>
#include <system_error>

namespace net {
    TcpListener::TcpListener(std::string& address, uint16_t port, int backlog, bool reuse_addr) {
        addrinfo hints{};
        hints.ai_family   = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags    = AI_PASSIVE;

        addrinfo* results = nullptr;
        std::string port_str = std::to_string(port);

        int err = getaddrinfo(address.empty() ? nullptr : address.c_str(),
                            port_str.c_str(), &hints, &results);
        if (err != 0) {
            throw std::runtime_error(std::string("getaddrinfo: ") + gai_strerror(err));
        }

        std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> addrinfo_guard(results, freeaddrinfo);

        for (auto* p = results; p != nullptr; p = p->ai_next) {
            try {
                Socket sock(p->ai_family, p->ai_socktype, p->ai_protocol);
                sock.set_reuse_address(reuse_addr);
                sock.bind(p->ai_addr, p->ai_addrlen);
                sock.listen(backlog);

                socket = std::move(sock);
                return;
            } catch (...) {
                continue;
            }
        }

        throw std::system_error(EADDRNOTAVAIL, std::system_category(),
                                "Не удалось привязать сокет: все адреса заняты или недоступны");
    }


    std::pair<Socket, ClientAddress> TcpListener::accept() {
        return socket.accept();
    }

    void TcpListener::close() noexcept {
        socket.close();
    }

    bool TcpListener::is_open() const noexcept {
        return socket.is_valid();
    }

    int TcpListener::native_handle() const noexcept {
        return socket.native_handle();
    }
}
