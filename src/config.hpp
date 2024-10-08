#ifndef BACKEND_CONFIG_HPP
#define BACKEND_CONFIG_HPP

#include "spdlog/spdlog.h"
#include <mutex>
#include "memory"
#include "yaml-cpp/yaml.h"
#include "openssl/rsa.h"
#include "iostream"

class svrConf : public std::enable_shared_from_this<svrConf> {
public:
    static std::shared_ptr<svrConf> instance() {
        std::call_once(_flag, []() {
            _instance.reset(new svrConf);
            try {
                YAML::Node conf = YAML::LoadFile("./config.yaml");
                _instance->secret = conf["server"]["secret"].as<std::string>();
                _instance->port = conf["server"]["port"].as<int16_t>();
                _instance->concurrency = conf["server"]["concurrency"].as<uint8_t>();
                _instance->debug = conf["server"]["debug"].as<bool>();

            } catch (std::exception &e) {
                spdlog::critical("load config failed: {}", e.what());
                std::exit(1);
            }
        });
        return _instance->shared_from_this();
    }

    svrConf(const svrConf &) = delete;

    svrConf &operator=(const svrConf &) = delete;

    std::string secret;

    uint16_t port = 8000;

    uint8_t concurrency = 0;

    bool debug = false;

private:
    svrConf() = default;
    // yaml 配置
    YAML::Node _conf;

    static std::shared_ptr<svrConf> _instance;
    static std::once_flag _flag;
};

std::shared_ptr<svrConf> svrConf::_instance;
std::once_flag svrConf::_flag;

#endif //BACKEND_CONFIG_HPP
