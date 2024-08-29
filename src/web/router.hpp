
#ifndef BACKEND_ROUTER_HPP
#define BACKEND_ROUTER_HPP

#include "init.hpp"
#include "middleware.hpp"
#include "unordered_map"

// 路由注册基类
class base_route : public std::enable_shared_from_this<base_route> {
public:
    base_route() = delete;

    explicit base_route(const char *r) : r(r) {};

    // get请求
    virtual void get_method(r_context &ctx) const {
        ctx.res.result(http::status::not_found);
        ctx.res.body() = "Not Found";
        ctx.exited = true;
    };

    // post请求
    virtual void post_method(r_context &ctx) const {
        ctx.res.result(http::status::not_found);
        ctx.res.body() = "Not Found";
        ctx.exited = true;
    };

    virtual void delete_method(r_context &ctx) const {
        ctx.res.result(http::status::not_found);
        ctx.res.body() = "Not Found";
        ctx.exited = true;
    }

    virtual void update_method(r_context &ctx) const {
        ctx.res.result(http::status::not_found);
        ctx.res.body() = "Not Found";
        ctx.exited = true;
    }

    // 路由函数处理逻辑
    virtual void operator()(r_context &ctx) const final {
        // 前置中间件
        for (auto &item: f_middle) {
            (*item)(ctx);
            if (ctx.exited) { return; };
        }
        // 根据请求方式选取处理逻辑
        if (ctx.method.empty() | ctx.method == "GET") {
            get_method(ctx);
        } else if (ctx.method == "POST") {
            post_method(ctx);
        } else if (ctx.method == "DELETE") {
            delete_method(ctx);
        } else if (ctx.method == "UPDATE") {
            update_method(ctx);
        } else {
            ctx.res.result(http::status::not_found);
            ctx.res.body() = "Not Found";
            ctx.exited = true;
        }
        // 后置中间件
        for (auto &item: b_middle) {
            if (ctx.exited) { return; };
            (*item)(ctx);
        }
    };

    std::string r;
    // 后置逻辑中间件
    std::vector<std::shared_ptr<middleware>> f_middle;
    // 前置逻辑中间件
    std::vector<std::shared_ptr<middleware>> b_middle;
};

// 路由组类
class group : public std::enable_shared_from_this<group> {
public:
    group() = delete;

    template<class... Args>
    explicit group(const char *r, const Args &...args):r(r) {
        routers(args...);
    }

    template<class T>
    void route(std::shared_ptr<T> item) {
        if (std::is_base_of<base_route, T>::value) {
            routers.push_back(item);
            return;
        }
        throw webException("valid type of route register");
    }

    std::vector<std::shared_ptr<base_route>> routers;

    // 后置逻辑中间件
    std::vector<std::shared_ptr<middleware>> f_middle;
    // 前置逻辑中间件
    std::vector<std::shared_ptr<middleware>> b_middle;
    std::string r;
};


// 路由管理器
class handler : public std::enable_shared_from_this<handler> {
public:
    static std::shared_ptr<handler> instance() {
        std::call_once(_flag, []() {
            _instance.reset(new handler);
        });
        return _instance->shared_from_this();
    }

    handler(const handler &) = delete;

    handler &operator=(const handler &) = delete;


    template<class ...Args>
    void routes(Args ...args) {
        route(args...);
    }

    // 未找到则返回空指针
    std::shared_ptr<base_route> operator[](const std::string &key) {
        auto it = _routes.find(key);
        if (it == _routes.end()) {
            std::shared_ptr<base_route> temp;
            return temp;
        }
        return it->second->shared_from_this();
    };

private:
    handler() = default;

    // 注册路由函数
    template<class T>
    void route(const std::shared_ptr<T> &item) {
        if (std::is_base_of<base_route, T>::value) {
            _routes[item->r] = item;
            return;
        }
        throw webException("valid type of route register");
    }

    void group(const std::shared_ptr<group> &p) {

    }

    std::unordered_map<std::string, std::shared_ptr<base_route>> _routes;

    static std::shared_ptr<handler> _instance;
    static std::once_flag _flag;
};

std::shared_ptr<handler> handler::_instance;
std::once_flag handler::_flag;

#endif //BACKEND_ROUTER_HPP