#ifndef BACKEND_SESSION_HPP
#define BACKEND_SESSION_HPP

#include "init.hpp"
#include "middlewares/url_parser.hpp"
#include "middleware.hpp"
#include "router.hpp"
#include "config.hpp"

class session : public std::enable_shared_from_this<session> {
public:
    explicit session(tcp::socket &soc) : soc(std::move(soc)) {

    };

    void do_read() {
        http::async_read(soc, buf, req,
                         [self = shared_from_this()](beast::error_code ec, std::size_t) {
                             if (ec) {
                                 spdlog::error("read failed: {}", ec.message());
                                 return;
                             }
                             self->ctx = std::make_shared<r_context>(self->req);
                             // 执行读取前中间件
                             for (auto &item: self->r_middle) {
                                 (*item)(*(self->ctx));
                                 if (self->ctx->exited) {
                                     break;
                                 }
                             }
                             // 获取路由逻辑类
                             auto r = (*(handler::instance()))[self->ctx->route];
                             // 404
                             if (!r) {
                                 self->ctx->res.result(http::status::not_found);
                                 self->ctx->res.body() = "Not Found";
                                 self->ctx->exited = true;
                             } else {
                                 // 执行处理逻辑
                                 try {
                                     (*r)(*(self->ctx));
                                 } catch (std::exception e) {
                                      self->ctx->exited = true;
                                      auto conf = svrConf::instance();
                                      self->ctx->res.result(http::status::bad_gateway);
                                     if (conf->debug){
                                         // debug时返回错误信息
                                         self->ctx->res.body() = e.what();
                                     }
                                     spdlog::error("controller failed: {}", e.what());
                                 }
                             }

                             self->do_write();
                         });
    }

private:
    void do_write() {
        for (auto &item: w_middle) {
            if (ctx->exited) {
                break;
            }
            (*item)(*ctx);
        }
        ctx->res.prepare_payload();
        http::async_write(soc, ctx->res,
                          [self = shared_from_this()](beast::error_code ec, std::size_t s) {
                              if (ec) {
                                  auto code = self->soc.shutdown(tcp::socket::shutdown_send, ec);
                                  spdlog::error("response failed: {}", code.message());
                                  return;
                              }
                              // 日志打印
                              spdlog::info("'{}', '{}', target:'{}', code:{}", self->ctx->method.to_string(),
                                           self->soc.remote_endpoint().address().to_string(), self->ctx->route,
                                           self->ctx->res.result_int());
                          });
    }

    tcp::socket soc;
    // 上下文
    std::shared_ptr<r_context> ctx;

    beast::flat_buffer buf;
    http::request<http::string_body> req;

    static std::vector<std::shared_ptr<middleware>> r_middle;
    static std::vector<std::shared_ptr<middleware>> w_middle;
};

std::vector<std::shared_ptr<middleware>> session::r_middle{std::make_shared<middlewares::url_parser>()};
std::vector<std::shared_ptr<middleware>> session::w_middle{};

#endif //BACKEND_SESSION_HPP
