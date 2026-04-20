#include "net/io.hpp"
#include <system_error>


namespace net {
    size_t read_some(Socket& connection, std::span<char> buffer) {
        auto res = ::recv(connection.native_handle(),
                            static_cast<void*>(buffer.data()),
                            buffer.size(), 0);
        if (res == -1) {
            if (errno == EINTR) return 0;

            if (errno == EPIPE || errno == ECONNRESET) {
                return 0;
            }

            throw std::system_error(errno, std::system_category(), "recv() failed");
        }
        return static_cast<size_t>(res);
    }

    void write_all(Socket& connection, std::span<const char> buffer) {
        size_t total_written = 0;
        while (total_written < buffer.size()) {
            auto res = ::send(connection.native_handle(),
                              reinterpret_cast<const void*>(buffer.data() + total_written),
                              buffer.size() - total_written, 0);

            if (res == -1) {
                if (errno == EINTR) continue;

                if (errno == EPIPE || errno == ECONNRESET) {
                    return;
                }

                throw std::system_error(errno, std::system_category(), "send() failed");
            }

            total_written += static_cast<size_t>(res);
            }
    }
}
