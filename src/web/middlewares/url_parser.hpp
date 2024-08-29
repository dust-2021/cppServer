
#ifndef BACKEND_URL_PARSER_HPP
#define BACKEND_URL_PARSER_HPP

#include "../init.hpp"
#include "../middleware.hpp"
#include "sstream"
#include "regex"
#include "boost/algorithm/string.hpp"


namespace middlewares {
    // 路由参数中间件
    class url_parser : public middleware {
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
                while (std::getline(param_s, temp, '&')) {
                    std::vector<std::string> v;
                    boost::split(v, temp, boost::is_any_of(std::string(1, '=')));
                    ctx.set<std::string>(v[0], v[1].c_str());
                }
            } else {
                ctx.route = org_route;
            }

            ctx.res.set(http::field::server, "Beast");
        };
    };

    const std::regex url_parser::url_match = std::regex{R"(^(.+?)\?((\w+?=\w+?&)*(\w+?=\w+?))$)"};
}

#endif //BACKEND_URL_PARSER_HPP
