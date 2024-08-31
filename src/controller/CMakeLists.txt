set(controller_dir
        index.hpp
        init_router.hpp
        api/login.hpp
)

add_library(controller INTERFACE ${controller_dir})

target_compile_features(controller INTERFACE cxx_std_17)