#ifndef BACKEND_INIT_ROUTER_HPP
#define BACKEND_INIT_ROUTER_HPP

#include "index.hpp"
#include "api/login.hpp"
#include "unordered_map"

namespace api {
    // 注册所有路由处理
    void init() {
        auto h = handler::instance();
        h->routes(std::make_shared<api::index>(),
                  std::make_shared<group>("/api", std::make_shared<api::login>())
        );
    }
}

#endif //BACKEND_INIT_ROUTER_HPP
