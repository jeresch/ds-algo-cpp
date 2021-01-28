add_library(ds-algo INTERFACE)
target_include_directories(ds-algo INTERFACE ${CMAKE_SOURCE_DIR}/include/)
target_compile_features(ds-algo INTERFACE cxx_std_20)
