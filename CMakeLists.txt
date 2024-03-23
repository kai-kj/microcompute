cmake_minimum_required(VERSION 3.20)
project(microcompute C)

# ==== microcompute ========================================================== #

add_library(
        microcompute SHARED
        src/buffer.c
        src/buffer_copier.c
        src/device.c
        src/instance.c
        src/misc.c
        src/program.c
        src/log.c
)

find_package(Vulkan REQUIRED)
target_include_directories(microcompute PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(microcompute PUBLIC ${Vulkan_LIBRARIES})

target_include_directories(microcompute PUBLIC include)
target_include_directories(microcompute PRIVATE src)

target_compile_options(
        microcompute PRIVATE
        -Wall -Wextra -Werror
        -Wno-unused-parameter -Wno-missing-braces -Wno-unused-function
)

# ==== microcompute_extra ==================================================== #

add_library(
        microcompute_extra SHARED
        src/hybrid_buffer.c
        src/extra.c
)

target_include_directories(microcompute_extra PUBLIC include)
target_include_directories(microcompute_extra PRIVATE src)

target_compile_options(
        microcompute_extra PRIVATE
        -Wall -Wextra -Werror
        -Wno-unused-parameter -Wno-missing-braces -Wno-unused-function
)

# ==== examples ============================================================== #

find_package(Vulkan REQUIRED COMPONENTS glslc)

add_executable(check_devs examples/check_devs.c ${PROJECT_BINARY_DIR}/check_devs.spv)
target_link_libraries(check_devs PRIVATE microcompute microcompute_extra)

add_executable(mandelbrot examples/mandelbrot.c ${PROJECT_BINARY_DIR}/mandelbrot.spv)
target_link_libraries(mandelbrot PRIVATE microcompute microcompute_extra)

target_compile_options(
        check_devs PRIVATE
        -g -O0 -Wall -Wextra -Werror
        -Wno-unused-parameter -Wno-missing-braces -Wno-unused-function
)

target_compile_options(
        mandelbrot PRIVATE
        -g -O0 -Wall -Wextra -Werror
        -Wno-unused-parameter -Wno-missing-braces -Wno-unused-function
)

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