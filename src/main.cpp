#include <string>
#include "net/tcp_listener.hpp"

int main() {
    std::string s = "0.0.0.0";
    s.clear();
    net::TcpListener listener(s, 8080);
}
