#ifndef BACKEND_WEB_SERVER_HPP
#define BACKEND_WEB_SERVER_HPP

#include <utility>

#include "init.hpp"
#include "middleware.hpp"
#include "routers.hpp"
#include "spdlog/spdlog.h"
#include "vector"

class server {
public:
    // 获取单例引用
    static server &instance() {
        static server ser;
        return ser;
    }

    server(const server &other) = delete;

    server(server &&other) = delete;


    // 注册路由回调函数
    void router(const char *url, base_route *r) {
        router_map[std::string(url)] = r;
    };

    //
    void handler(tcp::socket &_soc) {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;

        // 读取请求
        http::read(_soc, buffer, req);
        auto r_ctx = r_context(req);


        try {
            // 执行基础中间件
            for (const auto& item: base_middleware) {
                (*item)(r_ctx);
            }

            spdlog::info("receive '{}' from '{}', '{}'",r_ctx.method.to_string(),
                         _soc.remote_endpoint().address().to_string(), r_ctx.route);
            auto it = router_map.find(r_ctx.route);
            http::response<http::string_body> res;

            // 404 not found
            if (it == router_map.end()) {
                r_ctx.res.body() = "Not Found";
            } else {
                // 调用路由对象仿函数
                (*(it->second))(r_ctx);
            }

            r_ctx.res.prepare_payload();
            http::write(_soc, r_ctx.res);
            _soc.shutdown(tcp::socket::shutdown_send);
            _soc.close();
            return;
        } catch (const std::exception &exception) {
            spdlog::error("request failed: {}", exception.what());
            _soc.close();
        }
    }

    // 执行监听
    void run(uint16_t port_ = 8000, uint8_t concurrency = 1) {
        this->port = port_;
        this->ioc = new net::io_context{concurrency};
        this->ac = new tcp::acceptor{*this->ioc, tcp::endpoint(tcp::v4(), port)};
        this->soc = new tcp::socket{*ioc};
        spdlog::info("server at :{}", port);

        int i = 0;
        while (true) {
            if (i >= 5) {
                spdlog::error("server start finally failed.");
                return;
            }
            try {
                while (true) {
                    (*ac).accept(*soc);
                    handler(*soc);
                }
            }
            catch (const std::exception &exception) {
                spdlog::warn("server start failed: {}", exception.what());
                i++;
            }
        }
    };

    ~server(){
        soc->close();
        ac->close();
        ioc->stop();
        delete soc;
        delete ac;
        delete ioc;
        for (const auto& item: router_map) {
            delete item.second;
        }
        router_map.clear();
    }

private:

    server() {
        // 添加基本中间件
        auto temp = new base_parser;
        base_middleware.push_back(temp);

    };

    net::io_context *ioc = nullptr;
    tcp::socket *soc = nullptr;
    uint16_t port = 0;
    // 回调函数哈希表
    std::unordered_map<std::string, base_route*> router_map;
    tcp::acceptor *ac = nullptr;
    std::vector<middleware*> base_middleware;

};

#endif //BACKEND_WEB_SERVER_HPP
