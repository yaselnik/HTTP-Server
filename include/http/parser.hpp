#include "request.hpp"

namespace http {
    constexpr size_t MAX_HEADERS = 128;
    constexpr size_t MAX_HEADER_LEN = 8192;
    constexpr size_t MAX_BODY_SIZE = 10 * 1024 * 1024;

    [[nodiscard]] ParseResult parse(std::string_view data,
                                   Request& request) noexcept;
}
