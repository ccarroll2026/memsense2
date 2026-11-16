cmake_minimum_required(VERSION 3.15)
project(memsense2 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

find_package(GTest REQUIRED)

add_library(mscip MemsenseCommon.cpp serial.cpp)
add_executable(example example.cpp)
add_executable(my_tests test_main.cpp)

target_link_libraries(example PRIVATE mscip)
target_link_libraries(my_tests PRIVATE mscip gtest gtest_main)
# target_link_libraries(my_tests GTest::gtest GTest::gtest_main pthread)

target_include_directories(mscip PUBLIC MemsenseCommon.hpp structs.h)

option(MSCIP_BUILD_TESTS "Build unit tests" ON)
option(MSCIP_SANITIZE "Build with ASan and UBSan" OFF)

include(GoogleTest)
gtest_discover_tests(my_tests)