cmake_minimum_required(VERSION 3.15)
project(memsense2 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # -std=c++17, not gnu++17
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

option(MSCIP_BUILD_TESTS "Build unit tests" ON)
option(MSCIP_SANITIZE "Build with ASan and UBSan" OFF)

add_library(mscip src/MemsenseCommon.cpp)
add_executable(example example.cpp)

target_link_libraries(example PRIVATE mscip)

target_include_directories(mscip PUBLIC inc)

# Library-only rules from the spec. PRIVATE so tests and the example (which use exceptions/RTTI) are unaffected.
target_compile_options(mscip PRIVATE
    -Wall -Wextra -Wpedantic -Wconversion -Werror
    -fno-exceptions -fno-rtti)

if(MSCIP_SANITIZE)
    # PUBLIC so example and my_tests (which link mscip) get the same flags
    target_compile_options(mscip PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(mscip PUBLIC -fsanitize=address,undefined)
endif()

if(MSCIP_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()

    add_executable(my_tests tests/test_main.cpp)
    target_link_libraries(my_tests PRIVATE mscip gtest gtest_main)

    include(GoogleTest)
    gtest_discover_tests(my_tests)
endif()
