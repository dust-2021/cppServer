#include "web/routers.hpp"

int main() {
    auto ser = server("0.0.0.0");
    ser.run();
};
