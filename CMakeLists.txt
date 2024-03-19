cmake_minimum_required(VERSION 3.20)
project(microcompute C)

# ==== microcompute ========================================================== #

add_library(microcompute SHARED)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

find_package(Vulkan REQUIRED COMPONENTS glslc)
target_include_directories(microcompute PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(microcompute PUBLIC ${Vulkan_LIBRARIES})

target_include_directories(microcompute PUBLIC include)
target_include_directories(microcompute PRIVATE src)

target_sources(
        microcompute PRIVATE
        include/microcompute_vec.h
        src/buffer.c
        src/device.c
        src/instance.c
        src/misc.c
        src/program.c
)

add_custom_command(
        COMMENT "Generating microcompute_vec.h"
        OUTPUT ${PROJECT_SOURCE_DIR}/include/microcompute_vec.h
        COMMAND Python3::Interpreter ${PROJECT_SOURCE_DIR}/scripts/mc_vec.py > ${PROJECT_SOURCE_DIR}/include/microcompute_vec.h
        DEPENDS ${PROJECT_SOURCE_DIR}/scripts/mc_vec.py
)

# ==== examples ============================================================== #

add_executable(check_devs examples/check_devs.c ${PROJECT_BINARY_DIR}/check_devs.spv)
target_link_libraries(check_devs PRIVATE microcompute)

add_executable(mandelbrot examples/mandelbrot.c ${PROJECT_BINARY_DIR}/mandelbrot.spv)
target_link_libraries(mandelbrot PRIVATE microcompute)

set(GLSLC glslc -fshader-stage=compute)

add_custom_command(
        COMMENT "Compiling ${PROJECT_BINARY_DIR}/check_devs.spv"
        OUTPUT ${PROJECT_BINARY_DIR}/check_devs.spv
        COMMAND ${GLSLC} ${PROJECT_SOURCE_DIR}/examples/check_devs.glsl -o ${PROJECT_BINARY_DIR}/check_devs.spv
        DEPENDS ${PROJECT_SOURCE_DIR}/examples/check_devs.glsl
)

add_custom_command(
        COMMENT "Compiling ${PROJECT_BINARY_DIR}/mandelbrot.spv"
        OUTPUT ${PROJECT_BINARY_DIR}/mandelbrot.spv
        COMMAND ${GLSLC} ${PROJECT_SOURCE_DIR}/examples/mandelbrot.glsl -o ${PROJECT_BINARY_DIR}/mandelbrot.spv
        DEPENDS ${PROJECT_SOURCE_DIR}/examples/mandelbrot.glsl
)