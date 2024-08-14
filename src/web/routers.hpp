#ifndef  EXAMPLE_ROUTERS_HPP
#define  EXAMPLE_ROUTERS_HPP

#include "init.hpp"
#include "middleware.hpp"
#include "unordered_map"

// 路由注册基类
class base_route {
public:
    explicit base_route(const char *r) : r(r) {
    };

    virtual void get_method(r_context &ctx) {
        throw webException("valid method 'GET'");
    };

    virtual void post_method(r_context & ctx) {
        throw webException("valid method 'POST'");
    };

    virtual void operator()(r_context &ctx) final {
        for (auto &item: f_middle) {

            item(ctx);
        }

        if (ctx.method.empty() | ctx.method == "get"){
            get_method(ctx);
        } else if (ctx.method == "post"){
            post_method(ctx);
        }

        for (auto &item: b_middle) {
            item(ctx);
        }
    };

    std::string r;
    // 后置逻辑中间件
    std::vector<middleware> f_middle;
    // 前置逻辑中间件
    std::vector<middleware> b_middle;
};


#endif // EXAMPLE_ROUTERS_HPP