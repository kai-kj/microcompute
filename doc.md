
# `microcompute.h`

This library contains systems that can be used to easily run compute shaders
in GLSL.

## Types

```c
typedef enum mc_DebugLevel {
    MC_LVL_INFO,
    MC_LVL_LOW,
    MC_LVL_MEDIUM,
    MC_LVL_HIGH,
} mc_DebugLevel;
```

The severity of a debug message.

----

```c
typedef void(mc_debug_cb)(mc_DebugLevel level, const char* msg, void* arg);
```

The debug callback type passed to `mc_set_debug_cb()`.

- `level`: A `mc_DebugLevel` indicating the severity of the message
- `msg`: The message (NULL terminated), memory is handled the library, so
         dont free
- `arg`: A pointer to some user defined data passed to `mc_set_debug_cb()`

----

```c
typedef enum mc_BufferType {
    MC_BUFFER_UNIFORM,
    MC_BUFFER_STORAGE
} mc_BufferType;
```

Possible buffer types. `MC_BUFFER_TYPE_UNIFORM` is a uniform buffer, and
`MC_BUFFER_TYPE_STORAGE` is a SSBO.

----

```c
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
```

All basic value types supported by `mc_buffer_pack()` and
`mc_buffer_unpack()`.

----

```c
#define MC_ARRAY(len) (((len)&0xFF) << 24)
```

Use with `mc_ValueType` to indicate an array of values.

----

```c
typedef struct mc_Buffer mc_Buffer;
```

Buffer type.

----

```c
typedef struct mc_Program mc_Program;
```

Program type.

----

```c
typedef float mc_float;
typedef int32_t mc_int;
typedef uint32_t mc_uint;
```

Basic scalar data types that can be used in GLSL.

----

```c
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
```

The basic vector types.

----

## Functions

```c
char* mc_initialize(mc_debug_cb cb, void* arg);
```

Initialize microcompute.

- `cb`: A function to call when a error occurs, set to `NULL` to ignore
- `arg`: An pointer to pass to the debug callback, set to `NULL` to ignore
- returns: `NULL` if no errors, a null-terminated string otherwise (memory is
           handled by the library, so dont free)

----

```c
void mc_terminate();
```

Stop microcompute.

----

```c
double mc_finish_tasks();
```

Wait for all compute operations to finish.

- returns: The time spent waiting

----

```c
double mc_get_time();
```

Get the current time in seconds.

- returns: The current time

----

```c
mc_Buffer* mc_buffer_create_uniform(uint64_t size);
```

Create a uniform buffer.

- `size`: The initial size of the buffer, can be changed later
- returns: `NULL` on fail, a buffer otherwise

----

```c
mc_Buffer* mc_buffer_create_storage(uint64_t size);
```

Create a SSBO buffer.

- `size`: The initial size of the buffer, can be changed later
- returns: `NULL` on fail, a buffer otherwise

----

```c
void mc_buffer_destroy(mc_Buffer* buffer);
```

Destroy a buffer.

- `buffer`: A buffer

----

```c
mc_BufferType mc_buffer_get_type(mc_Buffer* buffer);
```

Get the current type of a buffer.

- `buffer`: A buffer
- returns: The type of the buffer

----

```c
uint64_t mc_buffer_get_size(mc_Buffer* buffer);
```

Get the current size of a buffer.

- `buffer`: A buffer
- returns: The size of the buffer (in bytes)

----

```c
void mc_buffer_set_size(mc_Buffer* buffer, uint64_t size);
```

Set the size of a buffer.

- `buffer`: A buffer
- `size`: The new size of the buffer (in bytes)

----

```c
void mc_buffer_write(
    mc_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
);
```

Write data to a buffer. Check the std140 and std430 layouts to make sure the
data is transferred correctly.

- `buffer`: A buffer
- `offset`: The offset from witch to start writing the data
- `size`: The size of the data to write
- `data`: A pointer to the data

----

```c
void mc_buffer_read(
    mc_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
);
```

Read data from a buffer. Check the std140 and std430 layouts to make sure the
data is transferred correctly.

- `buffer`: A buffer
- `offset`: The offset from witch to start reading the data
- `size`: The size of the data to read
- `data`: A pointer to write the data to (pre-allocated with enough space)

----

```c
#define mc_buffer_pack(buffer, ...)                                            \
    mc_buffer_pack__(buffer, __VA_ARGS__ __VA_OPT__(, ) 0);
```

Pack and write data to a buffer. Takes care of alignment automatically. No
support for structs, only basic variables and arrays. The maximum size of a
buffer that can be automatically packed is 1024 bytes.

The arguments should be formatted as follows:
1. Pass the type of the value with `mc_ValueType`
2. Pass a reference to the value
3. Repeat 1. and 2. for every value

Arrays can be specified by performing a bit-wise or (`|`) between the type of
the value (`mc_ValueType`) and `MC_ARRAY(len)`, where `len` is the length of
the array. The array can be passed directly (dont reference the array).

For example:
```c
int a = 12;
float b[] = {1.0, 2.0, 3.0};
mc_buffer_pack(buff, MC_INT, &a, MC_FLOAT | MC_ARRAY(3), &b)
```
will write the integer `12` and the float array `{1.0, 2.0, 3.0}` to the
buffer.

- `buffer`: A buffer
- `...`: Arguments explained above
- returns: The number of bytes written to the buffer, 0 on failure.

----

```c
#define mc_buffer_unpack(buffer, ...)                                          \
    mc_buffer_unpack__(buffer, __VA_ARGS__ __VA_OPT__(, ) 0);
```

Read and unpack data from a buffer. Takes care of alignment automatically. No
support for structs, only basic variables and arrays. The maximum size of a
buffer that can be automatically packed is 1024 bytes.

See `mc_buffer_pack()` for more info.

- `buffer`: A buffer
- `...`: Arguments explained above
- returns: The number of bytes read from the buffer, 0 on failure.

----

```c
mc_Program* mc_program_create(const char* code);
```

Create a program from a string.

- `code`: A null-terminated string of GLSL code
- returns: A program

----

```c
void mc_program_destroy(mc_Program* program);
```

Destroy a program.

- `program`: A program

----

```c
char* mc_program_check(mc_Program* program);
```

Check if there were any errors while compiling the shader code.

- `program`: A program
- returns: `NULL` if no errors, a null-terminated string otherwise (memory is
           handled by the library, so dont free)

----

```c
void mc_program_run_nonblocking(
    mc_Program* program,
    mc_uvec3 workgroupSize,
    mc_Buffer** buffers
);
```

Run a program on the GPU. The buffers passed to the program will have their
binding set depending on its index in the `buffers` array.

- `program`: A program
- `workgroupSize`: The number of work groups to dispatch in each dimension
- `buffers`: A null-terminated array of buffers to pass to the program

----

```c
double mc_program_run_blocking(
    mc_Program* program,
    mc_uvec3 workgroupSize,
    mc_Buffer** buffers
);
```

Run a program on the GPU. The buffers passed to the program will have their
binding set depending on its index in the `buffers` array.

Because this calls `mc_finish_tasks()` internally, it may significantly
affect performance if called many times in succession.

- `program`: A program
- `workgroupSize`: The number of work groups to dispatch in each dimension
- `buffers`: A null-terminated array of buffers to pass to the program
- returns: The time taken to run the program (in seconds)

----

```c
size_t mc_buffer_pack__(mc_Buffer* buffer, ...);
size_t mc_buffer_unpack__(mc_Buffer* buffer, ...);
```

Wrapped functions. Do not use them directly, use the wrapping macros.

----

## Licence

```
MIT License
Copyright (c) 2023 Kai Kitagawa-Jones

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
