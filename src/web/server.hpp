#ifndef EXAMPLE_SERVER_HPP
#define EXAMPLE_SERVER_HPP

#include <utility>

#include "init.hpp"
#include "spdlog/spdlog.h"
#include "stack"

class server {
public:
    static server &instance() {
        static server ser;
        return ser;
    }

    server(const server &other) = delete;

    server(server &&other) = delete;


    // 注册路由回调函数
    void router(const char *route,
                http::response<http::string_body>
                (*pFunction)(const server *const, http::request<http::string_body>)) {

        router_map[route] = [this, pFunction](http::request<http::string_body> &req) {
            return pFunction(this, req);
        };
    };

    void handler(tcp::socket &_soc) {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;

        http::read(_soc, buffer, req);
        auto target = req.target().data();
        spdlog::info("receive from '{}', '{}'", _soc.remote_endpoint().address().to_string(), target);
        try {
            auto it = router_map.find(target);
            http::response<http::string_body> res;
            if (it == router_map.end()) {
                res = http::response<http::string_body>{http::status::not_found, req.version()};

                res.set(http::field::server, "Beast");
                res.set(http::field::content_type, "text/plain");
                res.keep_alive(req.keep_alive());
                res.body() = "Not Found";

            } else {
                auto func = it->second;
                res = func(req);
            }

            res.prepare_payload();

            http::write(_soc, res);

            _soc.shutdown(tcp::socket::shutdown_send);
            _soc.close();
            return;
        } catch (const std::exception &exception) {
            spdlog::error("request failed: {}", exception.what());
            _soc.close();
        }
    }

    void run( uint16_t port_ = 8000, uint8_t concurrency = 1) {
        this->port = port_;
        this->ioc = new net::io_context{concurrency};
        this->ac = new tcp::acceptor{*this->ioc, tcp::endpoint(tcp::v4(), port)};
        this->soc = new tcp::socket{*ioc};
        spdlog::info("server at :{}", port);
        try {
            while (true) {
                (*ac).accept(*soc);
                handler(*soc);
            }
        }
        catch (const std::exception &exception) {
            spdlog::error("server start failed: {}", exception.what());

        }
    };

private:

    server() = default;

    net::io_context *ioc = nullptr;
    tcp::socket *soc = nullptr;
    uint16_t port = 0;
    // 回调函数哈希表
    std::unordered_map<const char *, std::function<http::response<http::string_body>(
            http::request<http::string_body> &)>> router_map;
    tcp::acceptor *ac = nullptr;
};

#endif //EXAMPLE_SERVER_HPP
