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
 * All basic value types supported by `mc_buffer_pack()` and
 * `mc_buffer_unpack()`.
 */
typedef enum mc_ValueType {
    MC_FLOAT = 0x00010101,
    MC_VEC2 = 0x00020202,
    MC_VEC3 = 0x00040303,
    MC_VEC4 = 0x00040404,
    MC_INT = 0x00010105,
    MC_IVEC2 = 0x00020206,
    MC_IVEC3 = 0x00040307,
    MC_IVEC4 = 0x00040408,
    MC_UINT = 0x00010109,
    MC_UVEC2 = 0x0002020A,
    MC_UVEC3 = 0x0004030B,
    MC_UVEC4 = 0x0004040C,
} mc_ValueType;

/** code
 * Use with `mc_ValueType` to indicate an array of values.
 */
#define MC_ARRAY(len) (((len)&0xFF) << 24)

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
 * The basic vector types.
 */

typedef struct mc_vec2 {
    mc_float x, y;
} mc_vec2;

typedef struct mc_vec3 {
    mc_float x, y, z;
} mc_vec3;

typedef struct mc_vec4 {
    mc_float x, y, z, w;
} mc_vec4;

typedef struct mc_ivec2 {
    mc_int x, y;
} mc_ivec2;

typedef struct mc_ivec3 {
    mc_int x, y, z;
} mc_ivec3;

typedef struct mc_ivec4 {
    mc_int x, y, z, w;
} mc_ivec4;

typedef struct mc_uvec2 {
    mc_uint x, y;
} mc_uvec2;

typedef struct mc_uvec3 {
    mc_uint x, y, z;
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
 * Create a uniform buffer.
 *
 * - `size`: The initial size of the buffer, can be changed later
 * - returns: `NULL` on fail, a buffer otherwise
 */
mc_Buffer* mc_buffer_create_uniform(uint64_t size);

/** code
 * Create a SSBO buffer.
 *
 * - `size`: The initial size of the buffer, can be changed later
 * - returns: `NULL` on fail, a buffer otherwise
 */
mc_Buffer* mc_buffer_create_storage(uint64_t size);

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
 * Pack and write data to a buffer. Takes care of alignment automatically. No
 * support for structs, only basic variables and arrays. The maximum size of a
 * buffer that can be automatically packed is 1024 bytes.
 *
 * The arguments should be formatted as follows:
 * 1. Pass the type of the value with `mc_ValueType`
 * 2. Pass a reference to the value
 * 3. Repeat 1. and 2. for every value
 *
 * Arrays can be specified by performing a bit-wise or (`|`) between the type of
 * the value (`mc_ValueType`) and `MC_ARRAY(len)`, where `len` is the length of
 * the array. The array can be passed directly (dont reference the array).
 *
 * For example:
 * ```c
 * int a = 12;
 * float b[] = {1.0, 2.0, 3.0};
 * mc_buffer_pack(buff, MC_INT, &a, MC_FLOAT | MC_ARRAY(3), &b)
 * ```
 * will write the integer `12` and the float array `{1.0, 2.0, 3.0}` to the
 * buffer.
 *
 * - `buffer`: A buffer
 * - `...`: Arguments explained above
 * - returns: The number of bytes written to the buffer, 0 on failure.
 */
#define mc_buffer_pack(buffer, ...)                                            \
    mc_buffer_pack__(buffer, __VA_ARGS__ __VA_OPT__(, ) 0);

/** code
 * Read and unpack data from a buffer. Takes care of alignment automatically. No
 * support for structs, only basic variables and arrays. The maximum size of a
 * buffer that can be automatically packed is 1024 bytes.
 *
 * See `mc_buffer_pack()` for more info.
 *
 * - `buffer`: A buffer
 * - `...`: Arguments explained above
 * - returns: The number of bytes read from the buffer, 0 on failure.
 */
#define mc_buffer_unpack(buffer, ...)                                          \
    mc_buffer_unpack__(buffer, __VA_ARGS__ __VA_OPT__(, ) 0);

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
 * - `workgroupSize`: The number of work groups to dispatch in each dimension
 * - `buffers`: A null-terminated array of buffers to pass to the program
 */
void mc_program_run_nonblocking(
    mc_Program* program,
    mc_uvec3 workgroupSize,
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
 * - `workgroupSize`: The number of work groups to dispatch in each dimension
 * - `buffers`: A null-terminated array of buffers to pass to the program
 * - returns: The time taken to run the program (in seconds)
 */

double mc_program_run_blocking(
    mc_Program* program,
    mc_uvec3 workgroupSize,
    mc_Buffer** buffers
);

/** code
 * Wrapped functions. Do not use them directly, use the wrapping macros.
 */

size_t mc_buffer_pack__(mc_Buffer* buffer, ...);
size_t mc_buffer_unpack__(mc_Buffer* buffer, ...);

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

#ifdef MICROCOMPUTE_IMPLEMENTATION

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define GLAD_GL_IMPLEMENTATION
#define GLAD_EGL_IMPLEMENTATION

#include "microcompute_egl.h"
#include "microcompute_gl.h"

struct mc_State {
    EGLDisplay disp;
    EGLContext ctx;
    mc_debug_cb* debug_cb;
    void* debug_cb_arg;
};

typedef struct mc_Program {
    GLint shader;
    GLint program;
    char* error;
} mc_Program;

typedef struct mc_Buffer {
    GLuint buffer;
    GLenum type;
} mc_Buffer;

static struct mc_State S;

#define MC_TYPE_SIZE(type) (((type) >> 8) & 0xFF)
#define MC_TYPE_ALIGN(type) (((type) >> 16) & 0xFF)
#define MC_TYPE_ARRAY_LEN(type) (((type) >> 24) & 0xFF)
#define MC_ALIGN_POS(pos, align)                                               \
    ((pos) % (align) != 0 ? pos + align - ((pos) % (align)) : pos)

static void mc_gl_debug_cb(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* arg
) {
    if (!S.debug_cb) return;

    mc_DebugLevel level;
    switch (severity) {
        case GL_DEBUG_SEVERITY_NOTIFICATION: level = MC_LVL_INFO; break;
        case GL_DEBUG_SEVERITY_LOW: level = MC_LVL_LOW; break;
        case GL_DEBUG_SEVERITY_MEDIUM: level = MC_LVL_MEDIUM; break;
        case GL_DEBUG_SEVERITY_HIGH: level = MC_LVL_HIGH; break;
        default: return;
    }

    uint32_t strLen = snprintf(NULL, 0, "GL: %s", message) + 1;
    char* str = malloc(strLen);
    snprintf(str, strLen, "GL: %s", message);

    S.debug_cb(level, str, S.debug_cb_arg);

    free(str);
}

static char* mc_get_shader_errors(GLuint shader) {
    int32_t success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success) return NULL;

    GLint len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    char* error = malloc(len);
    glGetShaderInfoLog(shader, len, NULL, error);

    return error;
}

static char* mc_get_program_errors(GLuint program) {
    int32_t success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success) return NULL;

    GLint len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    char* error = malloc(len);
    glGetProgramInfoLog(program, len, NULL, error);

    return error;
}

static size_t mc_buffer_iter(mc_Buffer* buffer, int upload, va_list args) {
    int data[256] = {0};
    int pos = 0;

    if (!upload) {
        uint64_t size = mc_buffer_get_size(buffer);
        mc_buffer_read(buffer, 0, size > 256 ? 256 : size, data);
    }

    while (pos < 256) {
        mc_BufferType type = va_arg(args, mc_ValueType);
        if (!type) break;

        int size = MC_TYPE_SIZE(type);
        int align = MC_TYPE_ALIGN(type);
        int arrLen = MC_TYPE_ARRAY_LEN(type);
        void* val = va_arg(args, void*);

        if (arrLen == 0) {
            pos = MC_ALIGN_POS(pos, align);
            if (upload) memcpy(&(data[pos]), val, size * sizeof(int));
            else memcpy(val, &(data[pos]), size * sizeof(int));
            pos += size;
        } else {
            if (buffer->type == GL_UNIFORM_BUFFER)
                align = MC_TYPE_ALIGN(MC_VEC4);

            for (int i = 0; i < arrLen; i++) {
                pos = MC_ALIGN_POS(pos, align);
                void* element = (int*)val + i * size;
                if (upload) memcpy(&(data[pos]), element, size * sizeof(int));
                else memcpy(element, &(data[pos]), size * sizeof(int));
                pos += size;
            }
        }
    }

    if (pos >= 256) return 0;

    // for (int i = 0; i < 256; i++) printf("[%03d]: %08X\n", i, data[i]);

    if (upload) {
        mc_buffer_set_size(buffer, pos * sizeof(int));
        mc_buffer_write(buffer, 0, pos * sizeof(int), data);
    }

    return pos * sizeof(int);
}

char* mc_initialize(mc_debug_cb cb, void* arg) {
    S.debug_cb = cb;
    S.debug_cb_arg = arg;

    if (!gladLoaderLoadEGL(NULL)) return "failed to load egl";
    S.disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (S.disp == EGL_NO_DISPLAY) return "failed to get egl disp";

    EGLint major, minor;
    if (!eglInitialize(S.disp, &major, &minor)) return "failed to init egl";
    if (major < 1 || (major == 1 && minor < 5)) return "egl version too low";

    if (!gladLoaderLoadEGL(S.disp)) return "failed to reload egl";
    if (!eglBindAPI(EGL_OPENGL_API)) return "failed to bind opengl to egl";

    EGLConfig eglCfg;
    if (!eglChooseConfig(
            S.disp,
            (EGLint[]){EGL_NONE},
            &eglCfg,
            1,
            &(EGLint){0}
        ))
        return "failed to choose egl config";

    S.ctx = eglCreateContext(
        S.disp,
        eglCfg,
        EGL_NO_CONTEXT,
        (EGLint[]){
            EGL_CONTEXT_MAJOR_VERSION,
            4,
            EGL_CONTEXT_MINOR_VERSION,
            3,
            EGL_CONTEXT_OPENGL_DEBUG,
            EGL_TRUE,
            EGL_NONE,
        }
    );

    if (S.ctx == EGL_NO_CONTEXT) return "failed to create egl context";

    if (!eglMakeCurrent(S.disp, EGL_NO_SURFACE, EGL_NO_SURFACE, S.ctx))
        return "failed to make egl context current";

    if (gladLoaderLoadGL() == 0) return "failed to load gl";

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(mc_gl_debug_cb, NULL);

    return NULL;
}

void mc_terminate() {
    if (S.ctx != 0) eglDestroyContext(S.disp, S.ctx);
    if (S.disp != 0) eglTerminate(S.disp);
}

double mc_finish_tasks() {
    double startTime = mc_get_time();
    glFinish();
    return mc_get_time() - startTime;
}

double mc_get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)(1000000 * tv.tv_sec + tv.tv_usec) / 1000000.0;
}

mc_Buffer* mc_buffer_create_uniform(uint64_t size) {
    mc_Buffer* buffer = malloc(sizeof *buffer);
    buffer->type = GL_UNIFORM_BUFFER;
    glGenBuffers(1, &buffer->buffer);
    mc_buffer_set_size(buffer, size);
    return buffer;
}

mc_Buffer* mc_buffer_create_storage(uint64_t size) {
    mc_Buffer* buffer = malloc(sizeof *buffer);
    buffer->type = GL_SHADER_STORAGE_BUFFER;
    glGenBuffers(1, &buffer->buffer);
    mc_buffer_set_size(buffer, size);
    return buffer;
}

void mc_buffer_destroy(mc_Buffer* buffer) {
    if (!buffer) return;
    if (buffer->buffer) glDeleteBuffers(1, &buffer->buffer);
    free(buffer);
}

mc_BufferType mc_buffer_get_type(mc_Buffer* buffer) {
    return buffer->type == GL_UNIFORM_BUFFER ? MC_BUFFER_UNIFORM
                                             : MC_BUFFER_STORAGE;
}

uint64_t mc_buffer_get_size(mc_Buffer* buffer) {
    GLint size;
    glBindBuffer(buffer->type, buffer->buffer);
    glGetBufferParameteriv(buffer->type, GL_BUFFER_SIZE, &size);
    return size;
}

void mc_buffer_set_size(mc_Buffer* buffer, uint64_t size) {
    glBindBuffer(buffer->type, buffer->buffer);
    glBufferData(buffer->type, size, NULL, GL_DYNAMIC_DRAW);
}

void mc_buffer_write(
    mc_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    glBindBuffer(buffer->type, buffer->buffer);
    glBufferSubData(buffer->type, offset, size, data);
}

void mc_buffer_read(
    mc_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    glBindBuffer(buffer->type, buffer->buffer);
    glGetBufferSubData(buffer->type, offset, size, data);
}

size_t mc_buffer_pack__(mc_Buffer* buffer, ...) {
    va_list args;
    va_start(args, buffer);
    size_t len = mc_buffer_iter(buffer, 1, args);
    va_end(args);
    return len;
}

size_t mc_buffer_unpack__(mc_Buffer* buffer, ...) {
    va_list args;
    va_start(args, buffer);
    size_t len = mc_buffer_iter(buffer, 0, args);
    va_end(args);
    return len;
}

mc_Program* mc_program_create(const char* code) {
    mc_Program* program = malloc(sizeof *program);
    program->error = NULL;

    program->shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(program->shader, 1, &code, NULL);
    glCompileShader(program->shader);

    if ((program->error = mc_get_shader_errors(program->shader)))
        return program;

    program->program = glCreateProgram();
    glAttachShader(program->program, program->shader);
    glLinkProgram(program->program);

    if ((program->error = mc_get_program_errors(program->program))) {
        glDeleteShader(program->shader);
        return program;
    }

    return program;
}

void mc_program_destroy(mc_Program* program) {
    if (!program) return;
    if (program->shader) glDeleteShader(program->shader);
    if (program->program) glDeleteProgram(program->program);
    if (program->error) free(program->error);
    free(program);
}

char* mc_program_check(mc_Program* program) {
    return program->error;
}

void mc_program_run_nonblocking(
    mc_Program* program,
    mc_uvec3 workgroupSize,
    mc_Buffer** buffers
) {
    if (program->error) return;
    glUseProgram(program->program);
    for (uint32_t i = 0; buffers[i] != NULL; i++) {
        mc_Buffer* buffer = buffers[i];
        glBindBufferBase(buffer->type, i, buffer->buffer);
    }
    glDispatchCompute(workgroupSize.x, workgroupSize.y, workgroupSize.z);
}

double mc_program_run_blocking(
    mc_Program* program,
    mc_uvec3 workgroupSize,
    mc_Buffer** buffers
) {
    double startTime = mc_get_time();
    mc_program_run_nonblocking(program, workgroupSize, buffers);
    mc_finish_tasks();
    return mc_get_time() - startTime;
}

#undef MC_TYPE_SIZE
#undef MC_TYPE_ALIGN
#undef MC_TYPE_ARRAY_LEN
#undef MC_ALIGN_POS

#endif // MICROCOMPUTE_IMPLEMENTATION
#endif // MICROCOMPUTE_H_INCLUDE_GUARD