#include "web/server.hpp"

int main() {
    auto svr = server::instance();
    svr->run();
};

