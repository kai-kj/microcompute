
# `microcompute.h`

This library contains utilities that can be used to easily run compute
SPIR-V shaders using vulkan. The library is built to be used with GLSL
shaders, although it should also work with other shader types.

## Types

```c
typedef enum mc_DebugLevel {
    MC_DEBUG_LEVEL_INFO,
    MC_DEBUG_LEVEL_LOW,
    MC_DEBUG_LEVEL_MEDIUM,
    MC_DEBUG_LEVEL_HIGH,
    MC_DEBUG_LEVEL_UNKNOWN,
} mc_DebugLevel;
```

The severity of a debug message.

----

```c
typedef void(mc_debug_cb)(
    mc_DebugLevel level,
    const char* source,
    const char* msg,
    void* arg
);
```

The debug callback type passed to `mc_set_debug_cb()`.

- `level`: A `mc_DebugLevel` indicating the severity of the message
- `source`: The message source (NULL terminated)
- `msg`: The message contents (NULL terminated)
- `arg`: The value passed to `debugArg` in `mc_state_create()`

----

```c
typedef float mc_float_t;
typedef int32_t mc_int_t;
typedef uint32_t mc_uint_t;
```

Basic scalar data types that can be used in GLSL.

----

```c
typedef struct mc_vec2 {
    mc_float_t x, y;
} mc_vec2_t;

typedef struct mc_vec3 {
    mc_float_t x, y, z;
} mc_vec3_t;

typedef struct mc_vec4 {
    mc_float_t x, y, z, w;
} mc_vec4_t;

typedef struct mc_ivec2 {
    mc_int_t x, y;
} mc_ivec2_t;

typedef struct mc_ivec3 {
    mc_int_t x, y, z;
} mc_ivec3_t;

typedef struct mc_ivec4 {
    mc_int_t x, y, z, w;
} mc_ivec4_t;

typedef struct mc_uvec2 {
    mc_uint_t x, y;
} mc_uvec2_t;

typedef struct mc_uvec3 {
    mc_uint_t x, y, z;
} mc_uvec3_t;

typedef struct mc_uvec4 {
    mc_uint_t x, y, z, w;
} mc_uvec4_t;
```

The basic vector types that can be used in GLSL.

----

```c
typedef struct mc_State mc_State_t;
```

The microcompute state type.

----

```c
typedef struct mc_Program mc_Program_t;
```

The program type.

----

## Functions

```c
const char* mc_debug_level_to_str(mc_DebugLevel level);
```

Convert a `mc_DebugLevel` enum to a human readable string.

- `level`: A `mc_DebugLevel` value
- returns: A human readable string (`NULL` terminated)

----

```c
mc_State_t* mc_state_create(mc_debug_cb* debug_cb, void* debugArg);
```

Create a `mc_State_t` object.

- `debug_cb`: A function to call when a error occurs, set to `NULL` to ignore
- `debugArg`: A value to pass to the debug callback, set to `NULL` to ignore
- returns: A reference to a `mc_State_t` object

----

```c
void mc_state_destroy(mc_State_t* self);
```

Destroy a `mc_State_t` object.

- `self`: A reference to a `mc_State_t` object

----

```c
bool mc_state_is_initialized(mc_State_t* self);
```

Checks whether a `mc_State_t` object has been successfully initialized.

- `self`: A reference to a `mc_State_t` object
- returns: `true` if successful, `false` otherwise

----

```c
mc_Program_t* mc_program_create(
    mc_State_t* state,
    uint64_t shaderSize,
    uint32_t* shader,
    uint32_t bufferCount,
    uint64_t* bufferSizes
);
```

Create a `mc_Program_t` object.

- `state`: A reference to a `mc_State_t` object
- `shaderSize`: The size of `shader`, in bytes
- `shader`: SPIR-V code
- `bufferCount`: The number of buffers to create
- `bufferSizes`: The sizes of the buffers to create
- returns: A reference to a `mc_Program_t` object

----

```c
void mc_program_destroy(mc_Program_t* self);
```

Destroy a `mc_Program_t` object.

- `self`: A reference to a `mc_Program_t` object

----

```c
bool mc_program_is_initialized(mc_Program_t* self);
```

Checks whether a `mc_Program_t` object has been successfully initialized.

- `self`: A reference to a `mc_Program_t` object
- returns: `true` if successful, `false` otherwise

----

```c
uint32_t mc_program_get_buffer_count(mc_Program_t* self);
```

Get the number of buffers in a `mc_Program_t` object.

- `self`: A reference to a `mc_Program_t` object
- returns: The number of buffers

----

```c
uint64_t mc_program_nth_buffer_get_size(mc_Program_t* self, uint32_t n);
```

Get the size of the `n`th buffer in a `mc_Program_t` object.

- `self`: A reference to a `mc_Program_t` object
- `n`: The buffer of interest
- returns: The size of the buffer

----

```c
uint64_t mc_program_nth_buffer_write(
    mc_Program_t* self,
    uint32_t n,
    uint64_t offset,
    uint64_t size,
    void* data
);
```

Write data to the `n`th buffer in a `mc_Program_t` object.

- `self`: A reference to a `mc_Program_t` object
- `n`: The buffer of interest
- `offset`: The offset from witch to start writing the data, in bytes
- `size`: The size of the data to write, in bytes
- `data`: The data to write
- returns: The number of bytes written

----

```c
uint64_t mc_program_nth_buffer_read(
    mc_Program_t* self,
    uint32_t n,
    uint64_t offset,
    uint64_t size,
    void* data
);
```

Read data from the `n`th buffer in a `mc_Program_t` object.

- `self`: A reference to a `mc_Program_t` object
- `n`: The buffer of interest
- `offset`: The offset from witch to start reading the data, in bytes
- `size`: The size of the data to read, in bytes
- `data`: The data to read to
- returns: The number of bytes read

----

```c
double mc_program_dispatch(mc_Program_t* self, mc_uvec3_t size);
```

Run a `mc_Program_t` object.

- `self`: A reference to a `mc_Program_t` object
- `size`: The number of work groups to dispatch in each dimension
- returns: The time taken waiting for the compute operation tio finish, in
           seconds

----

```c
double mc_get_time();
```

Get the current time.

- returns: The current time, in seconds

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
