include(FetchContent)

set(BOOST_INCLUDE_LIBRARIES program_options)
set(BOOST_ENABLE_CMAKE ON)

FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-cmake.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)

FetchContent_MakeAvailable(Boost)

add_executable(ConfigParser main.cpp)
target_link_libraries(ConfigParser PRIVATE config::config Boost::program_options)

# config.ini рядом с exe
add_custom_command(TARGET ConfigParser POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/config.ini
            $<TARGET_FILE_DIR:ConfigParser>/config.ini
    VERBATIM
)
