#include "http/response.hpp"

namespace http {
    Response ok(std::string body, std::string content_type) {
        Response resp;
        resp.status_code = 200;
        resp.message = "OK";
        resp.body = std::move(body);
        resp.headers.push_back({"Content-Type", std::move(content_type)});
        return resp;
    }

    Response created(std::string body) {
        Response resp;
        resp.status_code = 201;
        resp.message = "Created";
        resp.body = std::move(body);
        return resp;
    }

    Response bad_request(std::string error) {
        Response resp;
        resp.status_code = 400;
        resp.message = "Bad Request";
        resp.body = std::move(error);
        return resp;
    }

    Response not_found(std::string error) {
        Response resp;
        resp.status_code = 404;
        resp.message = "Not Found";
        resp.body = std::move(error);
        return resp;
    }

    Response internal_error(std::string error) {
        Response resp;
        resp.status_code = 500;
        resp.message = "Internal Server Error";
        resp.body = std::move(error);
        return resp;
    }

    Response method_not_allowed(std::string allowed_methods) {
        Response resp;
        resp.status_code = 405;
        resp.message = "Method Not Allowed";
        if (!allowed_methods.empty()) {
            resp.headers.push_back({"Allow", allowed_methods});
        }
        return resp;
    }
}
