set(http_dir
        web/init.hpp
        web/routers.hpp
        web/server.hpp
)
add_library(resource INTERFACE ${http_dir} )

target_compile_features(resource INTERFACE cxx_std_17)
