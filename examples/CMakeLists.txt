cmake_minimum_required(VERSION 3.20)

project(microcompute_examples)

find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)
add_subdirectory(../ microcompute)

add_executable(arrays src/arrays.c)
target_include_directories(arrays PRIVATE ../include include)
target_link_libraries(arrays PRIVATE microcomputeEGL OpenGL::OpenGL OpenGL::EGL)

add_executable(mandelbrot src/mandelbrot.c)
target_include_directories(mandelbrot PRIVATE ../include include)
target_link_libraries(mandelbrot PRIVATE microcomputeEGL OpenGL::OpenGL OpenGL::EGL)