#ifndef EXAMPLE_MIDDLEWARE_HPP
#define EXAMPLE_MIDDLEWARE_HPP

#include "init.hpp"
#include "map"
#include "regex"

class middleware {
public:

    // 子类必须重载仿函数
    virtual http::request<http::string_body> operator()(http::request<http::string_body> &req) = 0;
};

class parser : public middleware {
public:
    static const std::regex parse;

    http::request<http::string_body> operator()(http::request<http::string_body> &req) override {
        std::cmatch match;
        if (std::regex_search(req.target().data(), match, parse)){

        }
        return req;
    };
};

const auto parser::parse = std::regex{"(.+)\?(.+?=.+?)+"};
#endif //EXAMPLE_MIDDLEWARE_HPP
