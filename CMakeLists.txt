cmake_minimum_required(VERSION 3.31)

project(ConfigParser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

set(BOOST_INCLUDE_LIBRARIES program_options)
set(BOOST_ENABLE_CMAKE ON)

FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-cmake.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)

FetchContent_MakeAvailable(Boost)

add_library(config STATIC src/config.cpp)

target_include_directories(config PUBLIC include)

add_executable(ConfigParser main.cpp)
target_link_libraries(ConfigParser PRIVATE config Boost::program_options)

configure_file(config.ini ${CMAKE_CURRENT_BINARY_DIR}/config.ini COPYONLY)
