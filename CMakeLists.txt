cmake_minimum_required(VERSION 3.20)

project(microcompute)

file(GLOB_RECURSE SRC_FILES src/*.c)

add_library(microcomputeGL ${SRC_FILES})
target_include_directories(microcomputeGL PUBLIC include)
target_include_directories(microcomputeGL PRIVATE src)

add_library(microcomputeEGL ${SRC_FILES})
target_compile_definitions(microcomputeEGL PRIVATE MC_STANDALONE_MODE)
target_include_directories(microcomputeEGL PUBLIC include)
target_include_directories(microcomputeEGL PRIVATE src)