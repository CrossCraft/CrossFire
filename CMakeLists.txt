cmake_minimum_required(VERSION 3.10)

project(CrossFire VERSION 0.1)

file(GLOB_RECURSE HEADERS include/*.hpp)

add_executable(CrossFire src/main.cpp)
target_include_directories(CrossFire PUBLIC include)