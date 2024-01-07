
# `microcompute.h`

This library contains utilities that can be used to easily run SPIR-V compute
shaders using vulkan.

## Usage

Define `MICROCOMPUTE_IMPLEMENTATION` before including this file in one of
your C files to create the implementation. It should look something like:

```c
#include ...
#include ...

#define MICROCOMPUTE_IMPLEMENTATION
#include "microcompute.h"
```

In other C files, just include this file as normal.

See https://github.com/kal39/microcompute for more info.

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
typedef enum mc_DeviceType {
    MC_DEVICE_TYPE_DGPU, // discrete GPU
    MC_DEVICE_TYPE_IGPU, // integrated GPU
    MC_DEVICE_TYPE_VGPU, // virtual GPU
    MC_DEVICE_TYPE_CPU,
    MC_DEVICE_TYPE_OTHER,
} mc_DeviceType_t;
```

The type of a mc_Device_t.

----

```c
typedef void(mc_debug_cb)( //
    mc_DebugLevel_t lvl,
    const char* src,
    void* arg,
    const char* file,
    int line,
    const char* fmt,
    ...
);
```

The debug callback type passed to `mc_set_debug_cb()`.

- `lvl`: A `mc_DebugLevel` indicating the severity of the message
- `src`: The message source (NULL terminated)
- `arg`: The value passed to `debugArg` in `mc_instance_create()`
- `file`: The file the message originated from (NULL terminated)
- `line`: The line the message originated from
- `fmt`: The message formatting (same format as `printf()`)
- `...`: The arguments for the formatting

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
mc_DeviceType_t mc_device_get_type(mc_Device_t* self);
```

Get the type of the device (discrete gpu etc).

- `self`: A reference to a `mc_Device_t` object
- returns: The type of the GPU

----

```c
uint64_t mc_device_get_total_memory_size(mc_Device_t* self);
```

Get the total video memory of a device.

- `self`: A reference to a `mc_Device_t` object
- returns: The size of the video memory in bytes

----

```c
uint64_t mc_device_get_used_memory_size(mc_Device_t* self);
```

Get the used video memory of a device.

- `self`: A reference to a `mc_Device_t` object
- returns: The size of the used video memory in bytes

----

```c
char* mc_device_get_name(mc_Device_t* self);
```

Get the name of a device.

- `self`: A reference to a `mc_Device_t` object
- returns: A `NULL` terminated string containing the name of the device

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
mc_Buffer_t* mc_buffer_create_from(
    mc_Device_t* device,
    uint64_t size,
    void* data
);
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
mc_Program_t* mc_program_create(mc_Device_t* device, const char* fileName);
```

Create a `mc_Program_t` object.

- `device`: A reference to a `mc_Device_t` object
- `fileName`: The path to the shader code
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
#define mc_program_setup(self, entryPoint, pcSize, ...)                        \
    mc_program_setup__(self, entryPoint, pcSize, ##__VA_ARGS__, NULL)
```

Bind buffers to a `mc_Program_t` object.

- `self`: A reference to a `mc_Program_t` object
- `entryPoint`: The entry point name, generally `"main"`
- `pcSize`: The size of the push constant data, set to 0 to ignore
- `...`: A list of buffers to bind to the program

----

```c
double mc_program_run(
    mc_Program_t* self,
    uint32_t dimX,
    uint32_t dimY,
    uint32_t dimZ,
    void* pcData
);
```

Run a `mc_Program_t` object.

- `self`: A reference to a `mc_Program_t` object
- `dimX`: The number of local workgroups in the x dimension
- `dimY`: The number of local workgroups in the y dimension
- `dimZ`: The number of local workgroups in the z dimension
- `pcData`: A reference to the push constant data, pass `NULL` to ignore
- returns: The time taken waiting for the compute operation tio finish, in
           seconds, -1.0 on fail

----

```c
double mc_get_time();
```

Get the current time.

- returns: The current time, in seconds

----

```c
const char* mc_debug_level_to_str(mc_DebugLevel_t level);
```

Convert a `mc_DebugLevel_t` enum to a human readable string.

- `level`: A `mc_DebugLevel_t` value
- returns: A human readable string (`NULL` terminated)

----

```c
const char* mc_device_type_to_str(mc_DeviceType_t type);
```

Convert a `mc_DeviceType_t` enum to a human readable string.

- `type`: A `mc_DeviceType_t` value
- returns: A human readable string (`NULL` terminated)

----

```c
void mc_default_debug_cb( //
    mc_DebugLevel_t lvl,
    const char* src,
    void* arg,
    char const* file,
    int line,
    const char* fmt,
    ...
);
```

Default callback function that can be passed to `mc_instance_create()`. Just
prints all messages to stdout. Use as a template for your own callback.

See `mc_debug_cb` for more info about the arguments.

----

```c
void mc_program_setup__(
    mc_Program_t* self,
    const char* entryPoint,
    uint32_t pcSize,
    ...
);

#endif // MICROCOMPUTE_H_INCLUDE_GUARD
```

Wrapped functions. Don't use these directly, use their corresponding macros.

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
