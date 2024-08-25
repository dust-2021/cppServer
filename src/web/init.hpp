#ifndef  BACKEND_WEB_INIT_HPP
#define  BACKEND_WEB_INIT_HPP

#include "boost/beast.hpp"
#include "boost/beast/ssl.hpp"
#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"
#include "unordered_map"
#include "string"

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;

class webException : public std::logic_error {
public:
    explicit webException(const char *msg) :
            std::logic_error(msg) {

    }

    explicit webException(std::string &msg) :
            std::logic_error(msg) {

    }

    const char *what() {
        return std::logic_error::what();
    }
};


#endif // BACKEND_WEB_INIT_HPP

