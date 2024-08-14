//
// Created by libo on 24-8-14.
//

#ifndef BACKEND_BASE_HPP

#include "../web/routers.hpp"

namespace api {
    class index : public base_route {
    public:
        index(): base_route(""){};

        void get_method(r_context &ctx) override {
            ctx.res.body() = "hello beast";
        }

    private:

    };
}

#define BACKEND_BASE_HPP

#endif //BACKEND_BASE_HPP
