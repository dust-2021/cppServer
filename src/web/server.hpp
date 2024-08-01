#ifndef EXAMPLE_SERVER_HPP
#define EXAMPLE_SERVER_HPP
#include <utility>

#include "init.hpp"
#include "spdlog/spdlog.h"
#include "stack"

class server : public std::enable_shared_from_this<server> {
public:

    explicit server(const char *host, uint16_t port = 8000, uint8_t concurrency = 1) :
            host(host), port(port), ioc(net::io_context{concurrency}),
            ac(ioc, tcp::endpoint(tcp::v4(), port)),
            soc(ioc) {};

    server(const server &other) = delete;

    server(server &&other) = delete;

    ~server() {
        soc.close();
        ioc.stop();
    };

    // 注册路由回调函数
    void router(const char *route, void (*pFunction)(const std::shared_ptr<server> &,
                                                     http::request<http::string_body>)) {
        auto ptr = std::shared_ptr<server>(this);
        router_map[route] = std::move([ptr, pFunction](http::request<http::string_body> &body) {
            return pFunction(ptr, body);
        });
    };

    void handler(tcp::socket & _soc){
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(_soc, buffer, req);
        try {
            auto func = router_map[req.target().data()];
            func(req);
        } catch (const std::exception & exception) {
            spdlog::error("request failed: {}", exception.what());
            http::response<http::string_body> res{http::status::not_found, req.version()};
            res.set(http::field::server, "Beast");
            res.set(http::field::content_type, "text/plain");
            res.keep_alive(req.keep_alive());
            res.body() = exception.what();
            res.prepare_payload();

            http::write(_soc, res);

            _soc.shutdown(tcp::socket::shutdown_send);
            _soc.close();
        }
    }

    void run() {
        spdlog::info("server at {}:{}", host, port);
        try {
            while (true) {
                ac.accept(soc);
                handler(soc);
            }
        }
        catch (const std::exception &exception) {
            spdlog::error("server start failed: {}", exception.what());

        }
    };

private:
    net::io_context ioc;
    tcp::socket soc;
    const char *host;
    uint16_t port;
    // 回调函数哈希表
    std::unordered_map<const char *, std::function<void(http::request<http::string_body> &)>> router_map;
    std::stack<int> middleware;
    tcp::acceptor ac;
};

#endif //EXAMPLE_SERVER_HPP
