#ifndef BACKEND_WEB_MIDDLEWARE_HPP
#define BACKEND_WEB_MIDDLEWARE_HPP

#include "init.hpp"
#include "map"
#include "regex"
#include "spdlog/spdlog.h"
#include "nlohmann/json.hpp"
#include "sstream"
#include "boost/algorithm/string.hpp"

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

    bool exited = false;

    r_context() = delete;

    //
    explicit r_context(http::request<http::string_body> &req) : req(req) {
        res = http::response<http::string_body>{};
        method = req.method_string().data();
    }


    // 获取上下文变量
    template<class T>
    T get(const char *key, T &&default_v = T()) {
        auto it = context.find(key);
        if (it == context.end()) {
            return default_v;
        }
        return std::any_cast<T>(it->second);
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

/* 基础中间件
 * 获取路由参数放入r_context类上下文变量中，以字符串格式存储
 * */
class base_parser : public middleware {
public:
    static const std::regex url_match;

    void operator()(r_context &ctx) const override {
        std::cmatch match;
        auto org_route = ctx.req.target().data();
        // 解析路由参数
        if (std::regex_search(org_route, match, url_match)) {

            ctx.route = match[1].str();
            auto param_s = std::stringstream(match[2]);
            std::string temp;
            while (std::getline(param_s, temp, '&')){
                std::vector<std::string> v;
                boost::split(v, temp, boost::is_any_of(std::string(1, '=')));
                ctx.set<std::string>(v[0].c_str(), v[1].c_str());
            }
        } else {
            ctx.route = org_route;
        }

        ctx.res.set(http::field::server, "Beast");
    };
};

const std::regex base_parser::url_match = std::regex{R"(^(.+?)\?((\w+?=\w+?&)*(\w+?=\w+?))$)"};
#endif //BACKEND_WEB_MIDDLEWARE_HPP
