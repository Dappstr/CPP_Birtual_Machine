cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
    BM
    VERSION 1.0.0
    LANGUAGES CXX
)

add_executable(
    bm
    src/main.cpp
)

target_include_directories(bm PRIVATE include)
