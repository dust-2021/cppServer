
#ifndef BACKEND_AUTHENTICATION_HPP
#define BACKEND_AUTHENTICATION_HPP

#include "../middleware.hpp"
#include "openssl/aes.h"
#include "openssl/rand.h"
#include "cstring"


namespace middlewares {
    class token : public middleware {
    public:
        static AES_KEY key;

        void operator()(r_context &ctx) const override {
            auto t = ctx.req.find("Token");
            if (t == ctx.req.end()){

            }
        }
    };
}

#endif //BACKEND_AUTHENTICATION_HPP
