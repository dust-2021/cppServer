set(http_dir
        web/init.hpp
        web/server.hpp
        web/routers.hpp
)
add_library(resource INTERFACE ${http_dir} )

target_compile_features(resource INTERFACE cxx_std_17)
