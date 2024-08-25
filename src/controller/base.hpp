//
// Created by libo on 24-8-14.
//

#ifndef BACKEND_CONTROLLER_BASE_HPP
#define BACKEND_CONTROLLER_BASE_HPP

#include "../web/routers.hpp"

namespace api {
    class index : public base_route {
    public:
        index() : base_route("/") {};

        void get_method(r_context &ctx) const override {
            auto name = ctx.get<std::string>("name");
            if (!name.empty()) {
                ctx.res.body() = "hello " + name;
            } else {
                ctx.res.body() = "hello beast";
            }
        }

    private:

    };

}

#endif //BACKEND_CONTROLLER_BASE_HPP
