#include "http/serializer.hpp"

namespace http {
    std::string serialize(const Response& response){
        std::string resp;

        resp += "HTTP/1.1 ";
        resp += std::to_string(response.status_code);
        resp += ' ';
        resp += response.message;
        resp += "\r\n";

        bool has_content_length = false;
        bool has_connection = false;

        for (const auto& [key, val] : response.headers) {
            resp += key;
            resp += ": ";
            resp += val;
            resp += "\r\n";
            if (key == "Content-Length") has_content_length = true;
            if (key == "Connection") has_connection = true;
        }

        if (!has_connection) {
            resp += "Connection: keep-alive\r\n";
        }

        if (!has_content_length) {
            resp += "Content-Length: ";
            resp += std::to_string(response.body.size());
            resp += "\r\n";
        }

        resp += "\r\n";

        if (!response.body.empty()) {
            resp += response.body;
        }

        return resp;
    }
}
