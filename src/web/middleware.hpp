#ifndef EXAMPLE_MIDDLEWARE_HPP
#define EXAMPLE_MIDDLEWARE_HPP

#include "init.hpp"
#include "map"
#include "regex"
#include "nlohmann/json.hpp"

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
    std::string route;

    boost::string_view method;

    //
    explicit r_context(http::request<http::string_body> &req) : req(req) {
        res = http::response<http::string_body>{};
        method = req.method_string().data();
    }

    // 获取上下文变量
    template<class T>
    T get(const char *key, T &&default_v) {
        auto it = context.find(key);
        if (it == context.end()) {
            return default_v;
        }
        return static_cast<T>(it->second);
    }

    // 获取上下文变量
    template<class T>
    T get(const char *key, T &default_v) {
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

    // 设置上下文变量
    template<class T>
    void set(const char *key, T &value) {
        context[key] = std::any(value);
    }

    nlohmann::json json() {
        if (_json.empty()) {
            try {
                _json = nlohmann::json::parse(req.body().c_str());
            }
            catch (nlohmann::json::parse_error &error) {

                throw webException(error.what());
            }
        }
        return _json;
    };
private:
    // json 数据
    nlohmann::json _json;
};

class middleware {
public:

    // 子类必须重载仿函数
    virtual void operator()(r_context &ctx) = 0;
};

// 基础中间件
class base_parser : public middleware {
public:
    static const std::regex parse;

    void operator()(r_context &ctx) override {
        std::cmatch match;
        auto org_route = ctx.req.target().data();
        if (std::regex_search(org_route, match, parse)) {
            ctx.route = match[0].str();

            ctx.set<std::string>("", "");
        } else {
            ctx.route = org_route;
        }

        ctx.res.set(http::field::server, "Beast");
        ctx.res.set(http::field::content_type, "text/plain");
    };
};

const std::regex base_parser::parse = std::regex{"(.+)\?(.+?=.+?&)*(.+?=.+?)"};
#endif //EXAMPLE_MIDDLEWARE_HPP
