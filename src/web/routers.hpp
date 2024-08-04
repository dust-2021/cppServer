#ifndef  EXAMPLE_ROUTERS_HPP
#define  EXAMPLE_ROUTERS_HPP

#include "init.hpp"
#include "middleware.hpp"
#include "nlohmann/json.hpp"
#include "server.hpp"
#include "unordered_map"

// 请求上下文
template<typename T>
class r_context {
public:
    http::request<http::string_body> req;
    std::unordered_map<const char*, T> context;
};

class route{
public:
    route(const char* r, middleware& mid): r(r){

    };
    virtual http::response<http::string_body> operator()(http::request<http::string_body >) = 0;
    const char* r;
};



#endif // EXAMPLE_ROUTERS_HPP