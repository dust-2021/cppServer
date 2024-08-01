#include <utility>

#include "init.hpp"
#include "spdlog/spdlog.h"

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
                                                     http::request<http::dynamic_body>)) {
        auto ptr = std::shared_ptr<server>(this);
        router_map[route] = std::move([ptr, pFunction](http::request<http::dynamic_body> &body) {
            return pFunction(ptr, body);
        });
    };

    void run() {
        spdlog::info("server at {}:{}", host, port);
        try {
            while (true) {
                ac.accept();
                throw webException("initial failed");
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
    std::unordered_map<const char *, std::function<void(http::request<http::dynamic_body> &)>> router_map;
    tcp::acceptor ac;
};

