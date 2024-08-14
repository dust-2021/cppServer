#include "web/server.hpp"
#include "controller/base.hpp"

int main() {
    server& ser = server::instance();
    ser.router("/", new api::index());
    ser.run();
};
