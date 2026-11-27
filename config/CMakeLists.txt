add_library(config STATIC src/config.cpp)
add_library(config::config ALIAS config)

target_include_directories(config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
