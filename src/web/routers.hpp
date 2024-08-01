#ifndef  EXAMPLE_ROUTERS_HPP
#define  EXAMPLE_ROUTERS_HPP
#include "init.hpp"
#include "nlohmann/json.hpp"
#include "server.hpp"

void login(const std::shared_ptr<server> &ser, const http::request<http::dynamic_body> &req) {
    switch (req.method()) {
        case http::verb::get:
            break;
        default:
            throw webException("valid method");
    }


}


#endif // EXAMPLE_ROUTERS_HPP