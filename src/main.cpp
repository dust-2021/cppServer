#include "web/server.hpp"
#include "controller/init_router.hpp"

int main() {
    auto svr = server::instance();
    api::init();
    svr->run();
};
