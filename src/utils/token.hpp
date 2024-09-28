#ifndef BACKEND_TOKEN_HPP
#define BACKEND_TOKEN_HPP

#include "openssl/rsa.h"
#include "openssl/aes.h"
#include "string"

namespace utils {
    enum tokenType {
        RSA_TYPE,
        AES_TYPE
    };

    class token {
    public:
        explicit token(const std::string &type) {
            _type = type == "rsa" ? tokenType::RSA_TYPE : tokenType::AES_TYPE;
        }

        tokenType _type;

        static AES_KEY aes_key;
    private:

    };
}

#endif //BACKEND_TOKEN_HPP
