#include "net/io.hpp"
#include <system_error>


namespace net {
    size_t read_full_buffer(Socket& connection, std::span<char> buffer) {
        size_t total_read = 0;
        while (total_read < buffer.size()) {
            auto res = ::recv(connection.native_handle(),
                              reinterpret_cast<void*>(buffer.data() + total_read),
                              buffer.size() - total_read, 0);

            if (res == -1) {
                if (errno == EINTR) continue;
                throw std::system_error(errno, std::system_category(), "recv() failed");
            }

            if (res == 0) return total_read;

            total_read += res;
        }

        return total_read;
    }

    size_t read_some(Socket& connection, std::span<char> buffer) {
        auto res = ::recv(connection.native_handle(),
                            static_cast<void*>(buffer.data()),
                            buffer.size(), 0);
        if (res == -1) {
            if (errno == EINTR) return 0;
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
                throw std::system_error(errno, std::system_category(), "send() failed");
            }

            total_written += static_cast<size_t>(res);
            }
    }
}
