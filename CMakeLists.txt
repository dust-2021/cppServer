cmake_minimum_required(VERSION 3.10)

project(backend CXX)

set(CMAKE_CXX_STANDARD 20)
# 检测boost版本
find_package(Boost REQUIRED)
set(least_boost_version "1.66")
if (Boost_VERSION VERSION_LESS ${least_boost_version})
    message(FATAL_ERROR "boost version ${Boost_VERSION}, but at least ${least_boost_version}")
else ()
    message(STATUS "boost version ${Boost_VERSION}")
endif ()

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})


add_subdirectory(extern/json)
add_subdirectory(extern/spdlog)
add_subdirectory(src)
add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} nlohmann_json resource ${OPENSSL_LIBRARIES} spdlog)


