cmake_minimum_required(VERSION 3.10)

project(CrossFire VERSION 0.1)

file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(CrossFire ${HEADERS} ${SOURCES})
target_include_directories(CrossFire PUBLIC include)
target_compile_definitions(CrossFire PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_options(CrossFire PRIVATE -Wall -Wextra -Werror)