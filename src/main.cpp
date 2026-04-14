#include <iostream>
#include <vector>
#include <span>
#include <string>
#include <string_view>
#include "net/tcp_listener.hpp"
#include "net/io.hpp"

int main() {
    std::string address = "127.0.0.1";
    net::TcpListener listener(address, 8080);
    auto [client, addr] = listener.accept();
    std::cout << "Connected: " << addr.ip << ":" << addr.port << "\n";

    std::vector<char> buf(1024);
    size_t n = read_some(client, buf);
    std::cout << "Read: " << std::string_view(buf.data(), n) << "\n";

    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK";
    write_all(client, std::span(response));
}
