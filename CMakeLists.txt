cmake_minimum_required(VERSION 3.20)
project(microcompute C)

find_package(Vulkan REQUIRED COMPONENTS glslc)

add_compile_options(-Wall -Wextra -Werror -Wno-unused-parameter -Wno-missing-braces -Wno-unused-function)

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

target_include_directories(microcompute PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(microcompute PRIVATE ${Vulkan_LIBRARIES})

target_include_directories(microcompute PUBLIC include)
target_include_directories(microcompute PRIVATE src)

# ==== microcompute_extra ==================================================== #

add_library(
        microcompute_extra SHARED
        src/hybrid_buffer.c
        src/extra.c
)

target_include_directories(microcompute_extra PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(microcompute_extra PRIVATE ${Vulkan_LIBRARIES})

target_link_libraries(microcompute_extra PRIVATE microcompute)

target_include_directories(microcompute_extra PUBLIC include)
target_include_directories(microcompute_extra PRIVATE src)

# ==== examples ============================================================== #

set(GLSLC glslc -fshader-stage=compute)

# ---- check_devs ------------------------------------------------------------ #

add_executable(check_devs examples/check_devs.c ${PROJECT_BINARY_DIR}/check_devs.spv)
target_link_libraries(check_devs PRIVATE microcompute microcompute_extra)

add_custom_command(
        COMMENT "Compiling SPIR-V shader ${PROJECT_BINARY_DIR}/check_devs.spv"
        OUTPUT ${PROJECT_BINARY_DIR}/check_devs.spv
        COMMAND ${GLSLC} ${PROJECT_SOURCE_DIR}/examples/check_devs.glsl -o ${PROJECT_BINARY_DIR}/check_devs.spv
        DEPENDS ${PROJECT_SOURCE_DIR}/examples/check_devs.glsl
)

# ---- mandelbrot ------------------------------------------------------------ #

add_executable(mandelbrot examples/mandelbrot.c ${PROJECT_BINARY_DIR}/mandelbrot.spv)
target_link_libraries(mandelbrot PRIVATE microcompute microcompute_extra)

add_custom_command(
        COMMENT "Compiling SPIR-V shader ${PROJECT_BINARY_DIR}/mandelbrot.spv"
        OUTPUT ${PROJECT_BINARY_DIR}/mandelbrot.spv
        COMMAND ${GLSLC} ${PROJECT_SOURCE_DIR}/examples/mandelbrot.glsl -o ${PROJECT_BINARY_DIR}/mandelbrot.spv
        DEPENDS ${PROJECT_SOURCE_DIR}/examples/mandelbrot.glsl
)