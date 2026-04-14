#pragma once

#include "net/socket.hpp"
#include <span>
#include <cstddef>

namespace net {
    [[nodiscard]] size_t read_full_buffer(Socket& connection, std::span<char> buffer);
    [[nodiscard]] size_t read_some(Socket& connection, std::span<char> buffer);
    void write_all(Socket& connection, std::span<const char> buffer);
}
