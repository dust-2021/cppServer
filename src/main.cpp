#include "web/routers.hpp"

int main() {
    server& ser = server::instance();
    ser.run("0.0.0.0");
};
