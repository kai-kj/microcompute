#ifndef MICROCOMPUTE_H_INCLUDE_GUARD
#define MICROCOMPUTE_H_INCLUDE_GUARD

/** text
 * # `microcompute.h`
 *
 * This library contains systems that can be used to easily run compute shaders
 * in GLSL.
 */

#include <stdint.h>

// #include "microalgebra.h"

/** text
 * ## Types
 */

/** code
 * The severity of a debug message.
 */

typedef enum mc_DebugLevel {
    MC_LVL_INFO,
    MC_LVL_LOW,
    MC_LVL_MEDIUM,
    MC_LVL_HIGH,
} mc_DebugLevel;

/** code
 * The debug callback type passed to `mc_set_debug_cb()`.
 *
 * - `level`: A `mc_DebugLevel` indicating the severity of the message
 * - `msg`: The message (NULL terminated), memory is handled the library, so
 *          dont free
 * - `arg`: A pointer to some user defined data passed to `mc_set_debug_cb()`
 */

typedef void(mc_debug_cb)(mc_DebugLevel level, const char* msg, void* arg);

/** code
 * Possible buffer types. `MC_BUFFER_TYPE_UNIFORM` is a uniform buffer, and
 * `MC_BUFFER_TYPE_STORAGE` is a SSBO.
 */

typedef enum mc_BufferType {
    MC_BUFFER_UNIFORM,
    MC_BUFFER_STORAGE
} mc_BufferType;

/** code
 * Buffer type.
 */

typedef struct mc_Buffer mc_Buffer;

/** code
 * Program type.
 */

typedef struct mc_Program mc_Program;

/** code
 * Basic scalar data types that can be used in GLSL.
 */

typedef float mc_float;
typedef int32_t mc_int;
typedef uint32_t mc_uint;

/** code
 * These are the basic vector types. `vec3`, `ivec3`, and `uvec3` have an
 * additional component `_`, used to match the padding of the std430 layout in
 * GLSL.
 */

typedef struct mc_vec2 {
    mc_float x, y;
} mc_vec2;

typedef struct mc_vec3 {
    mc_float x, y, z, _;
} mc_vec3;

typedef struct mc_vec4 {
    mc_float x, y, z, w;
} mc_vec4;

typedef struct mc_ivec2 {
    mc_int x, y;
} mc_ivec2;

typedef struct mc_ivec3 {
    mc_int x, y, z, _;
} mc_ivec3;

typedef struct mc_ivec4 {
    mc_int x, y, z, w;
} mc_ivec4;

typedef struct mc_uvec2 {
    mc_uint x, y;
} mc_uvec2;

typedef struct mc_uvec3 {
    mc_uint x, y, z, _;
} mc_uvec3;

typedef struct mc_uvec4 {
    mc_uint x, y, z, w;
} mc_uvec4;

/** text
 * ## Functions
 */

/** code
 * Initialize microcompute.
 *
 * - `cb`: A function to call when a error occurs, set to `NULL` to ignore
 * - `arg`: An pointer to pass to the debug callback, set to `NULL` to ignore
 * - returns: `NULL` if no errors, a null-terminated string otherwise (memory is
 *            handled by the library, so dont free)
 */

char* mc_initialize(mc_debug_cb cb, void* arg);

/** code
 * Stop microcompute.
 */
void mc_terminate();

/** code
 * Wait for all compute operations to finish.
 *
 * - returns: The time spent waiting
 */
double mc_finish_tasks();

/** code
 * Get the current time in seconds.
 *
 * - returns: The current time
 */
double mc_get_time();

/** code
 * Create a buffer.
 *
 * - `type`: `MC_BUFFER_TYPE_UNIFORM` or `MC_BUFFER_TYPE_STORAGE`
 * - `size`: The initial size of the buffer, can be changed later
 * - returns: `NULL` on fail, a buffer otherwise
 */

mc_Buffer* mc_buffer_create(mc_BufferType type, uint64_t size);

/** code
 * Destroy a buffer.
 *
 * - `buffer`: A buffer
 */

void mc_buffer_destroy(mc_Buffer* buffer);

/** code
 * Get the current type of a buffer.
 *
 * - `buffer`: A buffer
 * - returns: The type of the buffer
 */

mc_BufferType mc_buffer_get_type(mc_Buffer* buffer);

/** code
 * Get the current size of a buffer.
 *
 * - `buffer`: A buffer
 * - returns: The size of the buffer (in bytes)
 */

uint64_t mc_buffer_get_size(mc_Buffer* buffer);

/** code
 * Set the size of a buffer.
 *
 * - `buffer`: A buffer
 * - `size`: The new size of the buffer (in bytes)
 */

void mc_buffer_set_size(mc_Buffer* buffer, uint64_t size);

/** code
 * Write data to a buffer. Check the std140 and std430 layouts to make sure the
 * data is transferred correctly.
 *
 * - `buffer`: A buffer
 * - `offset`: The offset from witch to start writing the data
 * - `size`: The size of the data to write
 * - `data`: A pointer to the data
 */

void mc_buffer_write(
    mc_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
);

/** code
 * Read data from a buffer. Check the std140 and std430 layouts to make sure the
 * data is transferred correctly.
 *
 * - `buffer`: A buffer
 * - `offset`: The offset from witch to start reading the data
 * - `size`: The size of the data to read
 * - `data`: A pointer to write the data to (pre-allocated with enough space)
 */

void mc_buffer_read(
    mc_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
);

/** code
 * Create a program from a string.
 *
 * - `code`: A null-terminated string of GLSL code
 * - returns: A program
 */

mc_Program* mc_program_create(const char* code);

/** code
 * Destroy a program.
 *
 * - `program`: A program
 */

void mc_program_destroy(mc_Program* program);

/** code
 * Check if there were any errors while compiling the shader code.
 *
 * - `program`: A program
 * - returns: `NULL` if no errors, a null-terminated string otherwise (memory is
 *            handled by the library, so dont free)
 */

char* mc_program_check(mc_Program* program);

/** code
 * Run a program on the GPU. The buffers passed to the program will have their
 * binding set depending on its index in the `buffers` array.
 *
 * - `program`: A program
 * - `workgroup_size`: The number of work groups to dispatch in each dimension
 * - `buffers`: A null-terminated array of buffers to pass to the program
 */

void mc_program_run_nonblocking(
    mc_Program* program,
    mc_uvec3 workgroup_size,
    mc_Buffer** buffers
);

/** code
 * Run a program on the GPU. The buffers passed to the program will have their
 * binding set depending on its index in the `buffers` array.
 *
 * Because this calls `mc_finish_tasks()` internally, it may significantly
 * affect performance if called many times in succession.
 *
 * - `program`: A program
 * - `workgroup_size`: The number of work groups to dispatch in each dimension
 * - `buffers`: A null-terminated array of buffers to pass to the program
 * - returns: The time taken to run the program (in seconds)
 */

double mc_program_run_blocking(
    mc_Program* program,
    mc_uvec3 workgroup_size,
    mc_Buffer** buffers
);

/** text
 * ## Licence
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

#endif // MICROCOMPUTE_H_INCLUDE_GUARD