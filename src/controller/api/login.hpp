#ifndef BACKEND_LOGIN_HPP
#define BACKEND_LOGIN_HPP
#include "../../web/router.hpp"
#include "nlohmann/json.hpp"

namespace api{

    class login: public base_route{
    public:
        login(): base_route("/login"){

        }
        void post_method(r_context& ctx) const override{
            auto j = ctx.json();
            ctx.data(j, 1, "wrong");
        }
    };
}

#endif //BACKEND_LOGIN_HPP
