set(http_dir
        web/init.hpp
        web/server.hpp
        web/middleware.hpp
        web/session.hpp

        config.hpp
        web/router.hpp
        web/middlewares/url_parser.hpp
        web/middlewares/authentication.hpp
        utils/token.hpp
)
add_subdirectory(controller)

add_library(resource INTERFACE ${http_dir} )
target_link_libraries(resource INTERFACE controller)

target_compile_features(resource INTERFACE cxx_std_17)
