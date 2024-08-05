#ifndef EXAMPLE_MIDDLEWARE_HPP
#define EXAMPLE_MIDDLEWARE_HPP

#include "init.hpp"
#include "map"
#include "regex"

// 请求上下文
class r_context {
public:
    // 请求体
    http::request<http::string_body> req;
    // 响应体
    http::response<http::string_body> res;
    // 上下文变量
    std::unordered_map<const char *, std::any> context;
    // 目标路由
    const char* route = "";

    const char* method;

    explicit r_context(http::request<http::string_body >& req): req(req){
        res = http::response<http::string_body>{};
        method = req.method_string().data();
    }

    // 获取上下文变量
    template<class T>
    T get(const char *key, T&& default_v) {
        auto it = context.find(key);
        if (it == context.end()) {
            return default_v;
        }
        return static_cast<T>(it->second);
    }

    // 设置上下文变量
    template<class T>
    void set(const char *key, T &&value) {
        context[key] = std::any(value);
    }

    bool exited = false;
};

class middleware {
public:

    // 子类必须重载仿函数
    virtual void operator()(r_context& ctx) = 0;
};

class parser : public middleware {
public:
    static const std::regex parse;

    void operator()(r_context &ctx) override {
        std::cmatch match;
        auto org_route = ctx.req.target().data();
        if (std::regex_search(org_route, match, parse)){

        } else {
            ctx.route = org_route;
        }
    };
};

const auto parser::parse = std::regex{"(.+)\?(.+?=.+?)+"};
#endif //EXAMPLE_MIDDLEWARE_HPP
