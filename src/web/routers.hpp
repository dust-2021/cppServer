#ifndef  EXAMPLE_ROUTERS_HPP
#define  EXAMPLE_ROUTERS_HPP

#include "init.hpp"
#include "middleware.hpp"
#include "server.hpp"
#include "unordered_map"

// 路由注册基类
class route {
public:
    explicit route(const char *r) : r(r) {
    };

    virtual void get() { };

    virtual void operator()(r_context &ctx) final {

    };

    const char *r;
    // 后置逻辑中间件
    std::vector<middleware> f_middle;
    // 前置逻辑中间件
    std::vector<middleware> b_middle;
};


#endif // EXAMPLE_ROUTERS_HPP