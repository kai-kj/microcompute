cmake_minimum_required(VERSION 3.20)
project(microcompute CXX)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

find_package(Vulkan REQUIRED)

add_subdirectory(external/spdlog)
add_subdirectory(external/fmt)

# add_compile_options(-Wall -Wextra -Werror -Wno-unused-parameter -Wno-missing-braces -Wno-unused-function)

# ==== microcompute ========================================================== #

add_library(
        microcompute SHARED
        src/instance.cpp
        src/device.cpp
        src/buffer.cpp
        src/vma_implementation.cpp
)

target_include_directories(microcompute PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(microcompute PRIVATE Vulkan::Vulkan)

target_include_directories(microcompute PRIVATE external/VulkanMemoryAllocator/include)

target_include_directories(microcompute PRIVATE external/fmt/include)
target_link_libraries(microcompute PUBLIC fmt::fmt)

target_include_directories(microcompute PUBLIC include)
target_include_directories(microcompute PRIVATE src)

# ==== microcompute_extra ==================================================== #

#add_library(
#        microcompute_extra SHARED
#        src/hybrid_buffer.c
#        src/extra.c
#)
#
#target_include_directories(microcompute_extra PRIVATE ${Vulkan_INCLUDE_DIRS})
#target_link_libraries(microcompute_extra PRIVATE Vulkan::Vulkan)
#target_link_libraries(microcompute_extra PRIVATE Vulkan::shaderc_combined)
#
#target_link_libraries(microcompute_extra PRIVATE microcompute)
#
#target_include_directories(microcompute_extra PUBLIC include)
#target_include_directories(microcompute_extra PRIVATE src)

## ==== examples ============================================================== #
#
## ---- check_devs ------------------------------------------------------------ #
#
#add_executable(check_devs examples/check_devs.c)
#target_link_libraries(check_devs PRIVATE microcompute microcompute_extra)
#
## ---- mandelbrot ------------------------------------------------------------ #
#
#add_executable(mandelbrot examples/mandelbrot.c)
#target_link_libraries(mandelbrot PRIVATE microcompute microcompute_extra)


# ---- test ------------------------------------------------------------------- #

add_executable(test examples/test.cpp)
target_link_libraries(test PRIVATE microcompute)