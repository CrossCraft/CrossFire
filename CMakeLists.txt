cmake_minimum_required(VERSION 3.10)

project(CrossFire VERSION 0.1)

file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

### Tracy profiler

# Only enable tracy if we are in debug mode or relwithdebinfo
if(NOT CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    message(STATUS "Tracy profiler disabled because we are not in debug mode")
    add_compile_definitions(TRACY_ENABLE=0)
else()
    add_compile_definitions(TRACY_ENABLE=1 TRACY_ON_DEMAND=1 TRACY_NO_EXIT=1)
endif()
add_library(tracy STATIC libs/tracy/public/TracyClient.cpp)

add_executable(CrossFire ${HEADERS} ${SOURCES})
target_include_directories(CrossFire PUBLIC include libs/tracy/public)
target_link_libraries(CrossFire PUBLIC tracy)
target_compile_definitions(CrossFire PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_options(CrossFire PRIVATE -Wall -Wextra -Werror)