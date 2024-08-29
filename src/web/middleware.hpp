#ifndef BACKEND_WEB_MIDDLEWARE_HPP
#define BACKEND_WEB_MIDDLEWARE_HPP

#include "init.hpp"
#include "map"
#include "spdlog/spdlog.h"
#include "nlohmann/json.hpp"
#include "string"

// 请求上下文
class r_context {
public:
    // 请求体
    http::request<http::string_body> req;
    // 响应体
    http::response<http::string_body> res;
    // 上下文变量
    std::unordered_map<std::string, std::any> context;
    // 目标路由
    std::string route;

    boost::string_view method;

    bool exited = false;

    r_context() = delete;

    //
    explicit r_context(http::request<http::string_body> &req) : req(std::move(req)) {
        res = http::response<http::string_body>(http::status::ok, this->req.version());
        res.set(http::field::server, "Beast");
        method = this->req.method_string().data();
    }


    // 获取上下文变量
    template<class T>
    T get(const std::string &key, T &&default_v = T()) {
        auto it = context.find(key);
        if (it == context.end()) {
            return default_v;
        }
        return std::any_cast<T>(it->second);
    }

    // 设置上下文变量
    template<class T>
    void set(const std::string & key, T &&value) {
        context[key] = std::any(value);
    }

    // 设置上下文变量
    template<class T>
    void set(const char *key, T &value) {
        context[key] = std::any(value);
    }

    nlohmann::json json() {
        auto ctx_type = req["content-type"];
        if (ctx_type.empty() || ctx_type != "json"){
            throw webException("request context isn't json type");
        };
        if (_json.empty()) {
            try {
                _json = nlohmann::json::parse(req.body().c_str());
            }
            catch (nlohmann::json::parse_error &error) {

                throw webException(std::string("failed parse json: ").append(error.what()));
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
    middleware() = default;
    middleware(const middleware&) = delete;

    middleware(middleware&&) = delete;
    // 子类必须重载仿函数
    virtual void operator()(r_context &ctx) const = 0;
};

#endif //BACKEND_WEB_MIDDLEWARE_HPP
