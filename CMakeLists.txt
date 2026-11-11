cmake_minimum_required(VERSION 3.15)
project(memsense2 VERSION 1.0 LANGUAGES CXX)
add_executable(memsense example.cpp MemsenseCommon.cpp)

target_include_directories(memsense PRIVATE MemsenseCommon.hpp structs.h)