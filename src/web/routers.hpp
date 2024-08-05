#ifndef  EXAMPLE_ROUTERS_HPP
#define  EXAMPLE_ROUTERS_HPP

#include "init.hpp"
#include "middleware.hpp"
#include "nlohmann/json.hpp"
#include "server.hpp"
#include "unordered_map"

// 请求上下文
class r_context {
public:
    http::request<http::string_body> req;
    std::unordered_map<const char *, std::any> context;

    template<class T>
    T get(const char *key, T default_v = NULL) {
        auto it = context.find(key);
        if (it == context.end()) {
            return default_v;
        }
        return static_cast<T>(it->second);
    }

    template<class T>
    void set(const char *key, T &&value) {
        context[key] = std::any(value);
    }

    bool exited = false;
};

// 路由注册基类
class route {
public:
    template<typename... Args>
    explicit route(const char *r) : r(r) {
    };



    virtual http::response<http::string_body> operator()(http::request<http::string_body>) final {
        auto res = http::response<http::string_body>{};
        return res;
    };

    const char *r;
    // 后置逻辑中间件
    std::vector<middleware> f_middle;
    // 前置逻辑中间件
    std::vector<middleware> b_middle;
};


#endif // EXAMPLE_ROUTERS_HPP