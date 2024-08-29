
#ifndef BACKEND_TOKEN_HPP
#define BACKEND_TOKEN_HPP

#include "../middleware.hpp"
#include "openssl/aes.h"
#include "openssl/rand.h"
#include "cstring"

namespace middlewares {
    class token : public middleware {
    public:
        static AES_KEY key;
        void operator()(r_context &ctx) const override {

        }
    };
}

#endif //BACKEND_TOKEN_HPP
