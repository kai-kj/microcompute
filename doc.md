
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
} mc_DebugLevel_t;
```

The severity of a debug message.

----

```c
typedef void(mc_debug_cb)(
    mc_DebugLevel_t level,
    const char* source,
    const char* msg,
    void* arg
);
```

The debug callback type passed to `mc_set_debug_cb()`.

- `level`: A `mc_DebugLevel` indicating the severity of the message
- `source`: The message source (NULL terminated)
- `msg`: The message contents (NULL terminated)
- `arg`: The value passed to `debugArg` in `mc_instance_create()`

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
typedef struct mc_Instance mc_Instance_t;
typedef struct mc_Device mc_Device_t;
typedef struct mc_Buffer mc_Buffer_t;
typedef struct mc_Program mc_Program_t;
```

Core microcompute types.

----

## Functions

```c
const char* mc_debug_level_to_str(mc_DebugLevel_t level);
```

Convert a `mc_DebugLevel_t` enum to a human readable string.

- `level`: A `mc_DebugLevel_t` value
- returns: A human readable string (`NULL` terminated)

----

```c
mc_Instance_t* mc_instance_create(mc_debug_cb* debug_cb, void* debugArg);
```

Create a `mc_Instance_t` object.

- `debug_cb`: A function to call when a error occurs, set to `NULL` to ignore
- `debugArg`: A value to pass to the debug callback, set to `NULL` to ignore
- returns: A reference to a `mc_Instance_t` object

----

```c
void mc_instance_destroy(mc_Instance_t* self);
```

Destroy a `mc_Instance_t` object.

- `self`: A reference to a `mc_Instance_t` object

----

```c
bool mc_instance_is_initialized(mc_Instance_t* self);
```

Checks whether a `mc_Instance_t` object has been successfully initialized.

- `self`: A reference to a `mc_Instance_t` object
- returns: `true` if successful, `false` otherwise

----

```c
uint32_t mc_instance_get_device_count(mc_Instance_t* self);
```

Get the number of available `mc_Device_t` objects.

- `self`: A reference to a `mc_Instance_t` object
- returns: The number of devices

----

```c
mc_Device_t** mc_instance_get_devices(mc_Instance_t* self);
```

Get a list of of available `mc_Device_t` objects.

- `self`: A reference to a `mc_Instance_t` object
- returns: A list of devices

----

```c
bool mc_device_is_discrete_gpu(mc_Device_t* self);
```

Check if a `mc_Device_t` object is a discrete GPU.

- `self`: A reference to a `mc_Device_t` object
- returns: `true` if it is, `false` otherwise

----

```c
bool mc_device_is_integrated_gpu(mc_Device_t* self);
```

Check if a `mc_Device_t` object is a integrated GPU.

- `self`: A reference to a `mc_Device_t` object
- returns: `true` if it is, `false` otherwise

----

```c
mc_Buffer_t* mc_buffer_create(mc_Device_t* device, uint64_t size);
```

Create a `mc_Buffer_t` object.

- `device`: A reference to a `mc_Device_t` object
- `size`: The size of the buffer
- returns: A reference to a `mc_Buffer_t` object

----

```c
mc_Buffer_t* mc_buffer_from(mc_Device_t* device, uint64_t size, void* data);
```

Create a `mc_Buffer_t` object and initializes with some data.

- `device`: A reference to a `mc_Device_t` object
- `size`: The size of the buffer
- `data`: A reference to the data the initialize the buffer with
- returns: A reference to a `mc_Buffer_t` object

----

```c
void mc_buffer_destroy(mc_Buffer_t* self);
```

Destroy a `mc_Buffer_t` object.

- `self`: A reference to a `mc_Buffer_t` object

----

```c
bool mc_buffer_is_initialized(mc_Buffer_t* self);
```

Checks whether a `mc_Buffer_t` object been successfully initialized.

- `self`: A reference to a `mc_Buffer_t` object

----

```c
uint64_t mc_buffer_get_size(mc_Buffer_t* self);
```

Get the size of a `mc_Buffer_t` object.

- `self`: A reference to a `mc_Buffer_t` object
- returns: The size, in bytes

----

```c
uint64_t mc_buffer_write(
    mc_Buffer_t* self,
    uint64_t offset,
    uint64_t size,
    void* data
);
```

Write data to a `mc_Buffer_t` object.

- `self`: A reference to a `mc_Buffer_t` object
- `offset`: The offset from witch to start writing the data, in bytes
- `size`: The size of the data to write, in bytes
- `data`: The data to write
- returns: The number of bytes written

----

```c
uint64_t mc_buffer_read(
    mc_Buffer_t* self,
    uint64_t offset,
    uint64_t size,
    void* data
);
```

Read data from a `mc_Buffer_t` object.

- `self`: A reference to a `mc_Buffer_t` object
- `offset`: The offset from witch to start reading the data, in bytes
- `size`: The size of the data to read, in bytes
- `data`: Where to read the data to
- returns: The number of bytes read

----

```c
mc_Program_t* mc_program_create(
    mc_Device_t* device,
    const char* fileName,
    const char* entryPoint,
    uint32_t bufferCount
);
```

Create a `mc_Program_t` object.

- `device`: A reference to a `mc_Device_t` object
- `fileName`: The path to the shader code
- `entryPoint`: The entry point name, generally `"main"`
- `bufferCount`: The number of buffers to bind
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
double mc_program_run(mc_Program_t* self, mc_uvec3_t dims, ...);
```

Run a `mc_Program_t` object.

- `self`: A reference to a `mc_Program_t` object
- `size`: The number of work groups to dispatch in each dimension
- `...`: A list of buffers to bind to the program, must be of the same length
         as the `bufferCount` value passed to `mc_program_create()`
- returns: The time taken waiting for the compute operation tio finish, in
           seconds, -1.0 on fail

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
