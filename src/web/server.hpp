#ifndef BACKEND_WEB_SERVER_HPP
#define BACKEND_WEB_SERVER_HPP

#include <utility>

#include "init.hpp"
#include "middleware.hpp"
#include "router.hpp"
#include "spdlog/spdlog.h"
#include "vector"
#include "session.hpp"
#include "../config.hpp"
#include "../controller/init_router.hpp"

class server : public std::enable_shared_from_this<server> {
public:
    // 获取单例
    static std::shared_ptr<server> instance() {
        std::call_once(_flag, []() {
            _instance.reset(new server);
        });
        return _instance->shared_from_this();
    }

    server(const server &other) = delete;

    server &operator=(const server &) = delete;

    // 执行监听
    void run() {
        this->port = svrConf::instance()->port;
        const size_t t_num = svrConf::instance()->concurrency == 0 ? std::thread::hardware_concurrency()
                                                                   : svrConf::instance()->concurrency;
        this->ioc = new net::io_context{static_cast<int>(t_num)};
        this->ac = new tcp::acceptor{*this->ioc, tcp::endpoint(tcp::v4(), port)};
        spdlog::info("server at :{}, thread: {}", port, t_num);

        // 加载路由处理器
        api::init();
        do_accept();

        // 多线程监听
        std::vector<std::thread> threads;
        for (std::size_t i = 0; i < t_num; ++i) {
            threads.emplace_back([this]() { ioc->run(); });
        }
        for (auto &thread: threads) {
            thread.join();
        }
    };

    ~server() {
        ac->close();
        ioc->stop();
        spdlog::info("server shutdown");
        delete ac;
        delete ioc;
    }

private:
    void do_accept() {
        (*ac).async_accept([self = shared_from_this()](beast::error_code ec, tcp::socket _soc) {
            if (!ec) {
                auto se = std::make_shared<session>(_soc);
                se->do_read();
            };
            self->do_accept();
        });
    }

    server() = default;

    // 单例指针
    static std::shared_ptr<server> _instance;
    // 单例创建flag
    static std::once_flag _flag;

    net::io_context *ioc = nullptr;
    uint16_t port = 0;
    tcp::acceptor *ac = nullptr;

};

std::shared_ptr<server> server::_instance;
std::once_flag server::_flag;

#endif //BACKEND_WEB_SERVER_HPP
