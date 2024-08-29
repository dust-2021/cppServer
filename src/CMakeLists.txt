set(http_dir
        web/init.hpp
        web/server.hpp
        web/middleware.hpp
        web/session.hpp

        controller/index.hpp
        config.hpp
        web/router.hpp
        web/middlewares/url_parser.hpp
        controller/init_router.hpp
        web/middlewares/token.hpp
)
add_library(resource INTERFACE ${http_dir} )

target_compile_features(resource INTERFACE cxx_std_17)
