cmake_minimum_required(VERSION 3.20)

project(microcompute)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SRC_FILES src/*.c)

add_library(microcompute ${SRC_FILES})
set_property(TARGET microcompute PROPERTY C_STANDARD 99)
target_include_directories(microcompute PUBLIC include)
target_include_directories(microcompute PRIVATE src)