cmake_minimum_required(VERSION 3.10)

project(CrossFire VERSION 0.1)

file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(CrossFire ${HEADERS} ${SOURCES})
target_include_directories(CrossFire PUBLIC include)