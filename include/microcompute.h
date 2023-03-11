/**
 * # `microcompute`
 *
 * [**Github**](https://github.com/kal39/microcompute)
 *
 * For examples, see
 * - [Arrays](
 *   https://github.com/kal39/microcompute/blob/master/examples/arrays.c)
 * - [Mandelbrot set](
 *   https://github.com/kal39/microcompute/blob/master/examples/mandelbrot.c)
 *
 * ## Documentation
 */

#ifndef MICROCOMPUTE_INCLUDE_GUARD
#define MICROCOMPUTE_INCLUDE_GUARD

#include <stdint.h>

#include "k_result.h"

/**
 * ### Structs
 */

/** code
 * Private microcompute types.
 */

typedef struct mc_Program mc_Program;
typedef struct mc_Buffer mc_Buffer;

/** code
 * Vector types, compatible with glsl vectors.
 */

// float vectors
typedef struct mc_vec2 {
    float x, y;
} mc_vec2;

typedef struct mc_vec3 {
    float x, y, z;
} mc_vec3;

typedef struct mc_vec4 {
    float x, y, z, w;
} mc_vec4;

// int vectors
typedef struct mc_ivec2 {
    int32_t x, y;
} mc_ivec2;

typedef struct mc_ivec3 {
    int32_t x, y, z;
} mc_ivec3;

typedef struct mc_ivec4 {
    int32_t x, y, z, w;
} mc_ivec4;

// uint vectors
typedef struct mc_uvec2 {
    uint32_t x, y;
} mc_uvec2;

typedef struct mc_uvec3 {
    uint32_t x, y, z;
} mc_uvec3;

typedef struct mc_uvec4 {
    uint32_t x, y, z, w;
} mc_uvec4;

/** code
 * Matrix types, compatible with glsl matrices.
 *
 * If `transpose` is set to true, the matrix values are in row major order,
 * else, the values are in column major order. It is passed as the `transpose`
 * argument to the `glUniformMatrix()` functions.
 */

typedef struct mc_mat22 {
    float values[4];
    k_Bool transpose;
} mc_mat22;

typedef struct mc_mat33 {
    float values[9];
    k_Bool transpose;
} mc_mat33;

typedef struct mc_mat44 {
    float values[16];
    k_Bool transpose;
} mc_mat44;

typedef struct mc_mat23 {
    float values[6];
    k_Bool transpose;
} mc_mat23;

typedef struct mc_mat32 {
    float values[6];
    k_Bool transpose;
} mc_mat32;

typedef struct mc_mat24 {
    float values[8];
    k_Bool transpose;
} mc_mat24;

typedef struct mc_mat42 {
    float values[8];
    k_Bool transpose;
} mc_mat42;

typedef struct mc_mat34 {
    float values[12];
    k_Bool transpose;
} mc_mat34;

typedef struct mc_mat43 {
    float values[12];
    k_Bool transpose;
} mc_mat43;

/**
 * ### Enums
 */

/** code
 * Debug message severity level.
 *
 * Passed as an argument to the debug callback function.
 */

typedef enum mc_DebugLevel {
    mc_DebugLevel_INFO,
    mc_DebugLevel_LOW,
    mc_DebugLevel_MEDIUM,
    mc_DebugLevel_HIGH,
} mc_DebugLevel;

/**
 * ### Core functionality
 */

/** code
 * Initialize the microcompute library.
 *
 * - returns: `k_Result` with `ok = K_TRUE` on success, `ok = K_FALSE`
 * otherwise
 */
k_Result mc_start();

/** code
 * Stops the microcompute library.
 *
 * - returns: `k_Result` with `ok = K_TRUE` on success, `ok = K_FALSE`
 * otherwise
 */
k_Result mc_stop();

/** code
 * Waits for all shader operations to finish.
 *
 * - returns: `k_Result` with `ok = K_TRUE` on success, `ok = K_FALSE`
 * otherwise
 */
k_Result mc_memory_barrier();

/**
 * ### Program (compute shader) management
 */

/** code
 * Create a program (compute shader) from a string.
 *
 * - `program`: The program struct to initialize
 * - `code`: A string containing the shader code
 * - `maxErrorLength`: Maximum length for shader errors
 * - `error`: A string with longer than `maxErrorLength` that will contain any
 * shader errors
 * - returns: A non-`NULL` pointer on success, `NULL` otherwise
 */
mc_Program* mc_program_from_string(
    const char* code,
    uint32_t maxErrorLength,
    char* error
);

/** code
 * Create a program (compute shader) from a file.
 *
 * - `program`: The program struct to initialize
 * - `code`: A string containing the path to the file containing the shader code
 * - `maxErrorLength`: Maximum length for shader errors
 * - `error`: A string with longer than `maxErrorLength` that will contain any
 * shader errors
 * - returns: A non-`NULL` pointer on success, `NULL` otherwise
 */
mc_Program* mc_program_from_file(
    const char* path,
    uint32_t maxErrorLength,
    char* error
);

/** code
 * Destroy a program.
 *
 * - `program`: The program to destroy
 * - returns: `k_Result` with `ok = K_TRUE` on success, `ok = K_FALSE`
 * otherwise
 */
k_Result mc_program_destroy(mc_Program* program);

/** code
 * Dispatch (run) a program.
 *
 * - `program`: The program to run
 * - `size`: The number of workgroups to be run in each dimension
 * - `bufferCount`: The number of buffers to pass to the program
 * - `buffers`: The buffers to pass to the program, the the buffers will be
 * bound according to their index in this array
 * - returns: `k_Result` with `ok = K_TRUE` on success, `ok = K_FALSE`
 * otherwise
 */
k_Result mc_program_dispatch(
    mc_Program* program,
    mc_ivec3 size,
    uint32_t bufferCount,
    mc_Buffer** buffers
);

/** code
 * Set the value of uniform value.
 *
 * - `program`: The program in which to set the uniform value
 * - `name`: The name of the uniform to set
 * - `value`: The value of the uniform
 * - returns: `k_Result` with `ok = K_TRUE` on success, `ok = K_FALSE`
 * otherwise
 */

// for float values
k_Result mc_program_set_float(mc_Program* program, char* name, float value);
k_Result mc_program_set_vec2(mc_Program* program, char* name, mc_vec2 value);
k_Result mc_program_set_vec3(mc_Program* program, char* name, mc_vec3 value);
k_Result mc_program_set_vec4(mc_Program* program, char* name, mc_vec4 value);

// for int values
k_Result mc_program_set_int(mc_Program* program, char* name, int32_t value);
k_Result mc_program_set_ivec2(mc_Program* program, char* name, mc_ivec2 value);
k_Result mc_program_set_ivec3(mc_Program* program, char* name, mc_ivec3 value);
k_Result mc_program_set_ivec4(mc_Program* program, char* name, mc_ivec4 value);

// for uint values
k_Result mc_program_set_uint(mc_Program* program, char* name, uint32_t value);
k_Result mc_program_set_uvec2(mc_Program* program, char* name, mc_uvec2 value);
k_Result mc_program_set_uvec3(mc_Program* program, char* name, mc_uvec3 value);
k_Result mc_program_set_uvec4(mc_Program* program, char* name, mc_uvec4 value);

// for matrix values
k_Result mc_program_set_mat22(mc_Program* program, char* name, mc_mat22 value);
k_Result mc_program_set_mat33(mc_Program* program, char* name, mc_mat33 value);
k_Result mc_program_set_mat44(mc_Program* program, char* name, mc_mat44 value);
k_Result mc_program_set_mat23(mc_Program* program, char* name, mc_mat23 value);
k_Result mc_program_set_mat32(mc_Program* program, char* name, mc_mat32 value);
k_Result mc_program_set_mat24(mc_Program* program, char* name, mc_mat24 value);
k_Result mc_program_set_mat42(mc_Program* program, char* name, mc_mat42 value);
k_Result mc_program_set_mat34(mc_Program* program, char* name, mc_mat34 value);
k_Result mc_program_set_mat43(mc_Program* program, char* name, mc_mat43 value);

/**
 * ### Buffer management
 */

/** code
 * Create a buffer (SSBO).
 *
 * - `buffer`: The buffer struct to initialize
 * - `size`: The size of the buffer
 * - returns: A non-`NULL` pointer on success, `NULL` otherwise
 */
mc_Buffer* mc_buffer_create(uint64_t size);

/** code
 * Destroy a buffer.
 *
 * - `buffer`: The buffer to destroy
 * - returns: `k_Result` with `ok = K_TRUE` on success, `ok = K_FALSE`
 * otherwise
 */
k_Result mc_buffer_destroy(mc_Buffer* buffer);

/** code
 * Resize a buffer.
 *
 * - `buffer`: The buffer to resize
 * - `binding`: The (new) size of the buffer
 * - returns: `k_Result` with `ok = K_TRUE` on success, `ok = K_FALSE`
 * otherwise
 */
k_Result mc_buffer_resize(mc_Buffer* buffer, uint64_t size);

/** code
 * Get the current size of a buffer.
 *
 * - `buffer`: The buffer to get the size of
 * - returns: The current size of the buffer
 * - returns: `k_Result` with `ok = K_TRUE` on success, `ok = K_FALSE`
 * otherwise
 */
k_Result mc_buffer_get_size(mc_Buffer* buffer, uint64_t* size);

/** code
 * Write data to a buffer. If `off` + `size` is larger than the size of the
 * buffer, the function call will fail.
 *
 * - `buffer`: The buffer to write to
 * - `off`: The offset at which to start writing data to, measured in bytes
 * - `size`: The size (length) of the data to be written, measured in bytes
 * - `data`: The data to write
 * - returns: `k_Result` with `ok = K_TRUE` on success, `ok = K_FALSE`
 * otherwise
 */
k_Result mc_buffer_write(
    mc_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
);

/** code
 * Read data from a buffer. If `off` + `size` is larger than the size of the
 * buffer, the function call will fail.
 *
 * - `buffer`: The buffer to read from
 * - `off`: The offset at which to start reading data from, measured in bytes
 * - `size`: The size (length) of the data to be read, measured in bytes
 * - `data`: A buffer to write the data into. Must be pre-allocated
 * - returns: `k_Result` with `ok = K_TRUE` on success, `ok = K_FALSE`
 * otherwise
 */
k_Result mc_buffer_read(
    mc_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
);

/**
 * ## License
 *
 * ```
 * MIT License
 * Copyright (c) 2023 Kai Kitagawa-Jones
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ```
 */

#endif // MICROCOMPUTE_INCLUDE_GUARD