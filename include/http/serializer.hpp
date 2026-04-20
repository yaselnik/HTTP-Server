#pragma once

#include "http/response.hpp"
#include <string>
#include <string_view>

namespace http {
    [[nodiscard]] std::string serialize(const Response& response);
}
