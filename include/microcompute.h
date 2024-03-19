#ifndef MC_H_INCLUDE_GUARD
#define MC_H_INCLUDE_GUARD

/** text
 * # `mc.h`
 *
 * This library contains utilities that can be used to easily run SPIR-V compute
 * shaders using vulkan.
 *
 * ## Usage
 *
 * Define `MC_IMPLEMENTATION` before including this file in one of
 * your C files to create the implementation. It should look something like:
 *
 * ```c
 * #include ...
 * #include ...
 *
 * #define MC_IMPLEMENTATION
 * #include "microcompute.h"
 * ```
 *
 * In other C files, just include this file as normal.
 *
 * See https://github.com/kal39/microcompute for more info.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/** text
 * ## Types
 */

/** code
 * The severity of a log message.
 */
typedef enum mc_LogLevel {
    MC_LOG_LEVEL_DEBUG,
    MC_LOG_LEVEL_INFO,
    MC_LOG_LEVEL_WARN,
    MC_LOG_LEVEL_ERROR,
    MC_LOG_LEVEL_UNKNOWN,
} mc_LogLevel_t;

/** code
 * The type of a mc_Device_t.
 */
typedef enum mc_DeviceType {
    MC_DEVICE_TYPE_DGPU, // discrete GPU
    MC_DEVICE_TYPE_IGPU, // integrated GPU
    MC_DEVICE_TYPE_VGPU, // virtual GPU
    MC_DEVICE_TYPE_CPU,
    MC_DEVICE_TYPE_OTHER,
} mc_DeviceType_t;

/** code
 * The log callback type passed to `mc_set_log_cb()`.
 *
 * - `lvl`: A `mc_LogLevel` indicating the severity of the message
 * - `src`: The message source (NULL terminated)
 * - `arg`: The value passed to `logArg` in `mc_instance_create()`
 * - `file`: The file the message originated from (NULL terminated)
 * - `line`: The line the message originated from
 * - `fmt`: The message formatting (same format as `printf()`)
 * - `...`: The arguments for the formatting
 */
typedef void(mc_log_cb)( //
    mc_LogLevel_t lvl,
    const char* src,
    void* arg,
    const char* file,
    int line,
    const char* fmt,
    ...
);

/** code
 * Core microcompute types.
 */
typedef struct mc_Instance mc_Instance_t;
typedef struct mc_Device mc_Device_t;
typedef struct mc_Buffer mc_Buffer_t;
typedef struct mc_Program mc_Program_t;

/** text
 * ## Functions
 */

/** code
 * Create a `mc_Instance_t` object.
 *
 * - `log_cb`: A function to call when a error occurs, set to `NULL` to ignore
 * - `logArg`: A value to pass to the log callback, set to `NULL` to ignore
 * - returns: A instance object on success, `NULL` on error
 */
mc_Instance_t* mc_instance_create(mc_log_cb* log_cb, void* logArg);

/** code
 * Destroy a `mc_Instance_t` object.
 *
 * - `self`: A reference to a `mc_Instance_t` object
 */
void mc_instance_destroy(mc_Instance_t* self);

/** code
 * Get the number of available `mc_Device_t` objects.
 *
 * - `self`: A reference to a `mc_Instance_t` object
 * - returns: The number of devices
 */
uint32_t mc_instance_get_device_count(mc_Instance_t* self);

/** code
 * Get a list of of available `mc_Device_t` objects.
 *
 * - `self`: A reference to a `mc_Instance_t` object
 * - returns: A list of devices
 */
mc_Device_t** mc_instance_get_devices(mc_Instance_t* self);

/** code
 * Get the type of the device (discrete gpu etc).
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: The type of the GPU
 */
mc_DeviceType_t mc_device_get_type(mc_Device_t* self);

/** code
 * Get the total video memory of a device.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: The size of the video memory in bytes
 */
uint64_t mc_device_get_total_memory_size(mc_Device_t* self);

/** code
 * Get the used video memory of a device.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: The size of the used video memory in bytes
 */
uint64_t mc_device_get_used_memory_size(mc_Device_t* self);

/** code
 * Get the max workgroup size (the max for x*y*z) of a device.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: The max workgroup size
 */
uint32_t mc_device_get_max_workgroup_size_total(mc_Device_t* self);

/** code
 * Get the max workgroup size (for each x, y, z) of a device.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: The max workgroup size, as a 3 element array (dont free)
 */
uint32_t* mc_device_get_max_workgroup_size_shape(mc_Device_t* self);

/** code
 * Get the max workgroup count (for each x, y, z) of a device.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: The max workgroup count, as a 3 element array
 */
uint32_t* mc_device_get_max_workgroup_count(mc_Device_t* self);

/** code
 * Get the name of a device.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: A `NULL` terminated string containing the name of the device
 */
char* mc_device_get_name(mc_Device_t* self);

/** code
 * Create a `mc_Buffer_t` object.
 *
 * - `device`: A reference to a `mc_Device_t` object
 * - `size`: The size of the buffer
 * - returns: A reference to a `mc_Buffer_t` object
 */
mc_Buffer_t* mc_buffer_create(mc_Device_t* device, uint64_t size);

/** code
 * Create a `mc_Buffer_t` object and initializes with some data.
 *
 * - `device`: A reference to a `mc_Device_t` object
 * - `size`: The size of the buffer
 * - `data`: A reference to the data the initialize the buffer with
 * - returns: A buffer object on success, `NULL` on error
 */
mc_Buffer_t* mc_buffer_create_from(
    mc_Device_t* device,
    uint64_t size,
    void* data
);

/** code
 * Destroy a `mc_Buffer_t` object.
 *
 * - `self`: A reference to a `mc_Buffer_t` object
 */
void mc_buffer_destroy(mc_Buffer_t* self);

/** code
 * Get the size of a `mc_Buffer_t` object.
 *
 * - `self`: A reference to a `mc_Buffer_t` object
 * - returns: The size, in bytes
 */
uint64_t mc_buffer_get_size(mc_Buffer_t* self);

/** code
 * Reallocate a `mc_Buffer_t` object.
 *
 * - `self`: A reference to a `mc_Buffer_t` object
 * - `size`: The new size of the buffer
 * - returns: The new buffer object on success, `NULL` on error
 */
mc_Buffer_t* mc_buffer_realloc(mc_Buffer_t* self, uint64_t size);

/** code
 * Write data to a `mc_Buffer_t` object.
 *
 * - `self`: A reference to a `mc_Buffer_t` object
 * - `offset`: The offset from witch to start writing the data, in bytes
 * - `size`: The size of the data to write, in bytes
 * - `data`: The data to write
 * - returns: The number of bytes written
 */
uint64_t mc_buffer_write(
    mc_Buffer_t* self,
    uint64_t offset,
    uint64_t size,
    void* data
);

/** code
 * Read data from a `mc_Buffer_t` object.
 *
 * - `self`: A reference to a `mc_Buffer_t` object
 * - `offset`: The offset from witch to start reading the data, in bytes
 * - `size`: The size of the data to read, in bytes
 * - `data`: Where to read the data to
 * - returns: The number of bytes read
 */
uint64_t mc_buffer_read(
    mc_Buffer_t* self,
    uint64_t offset,
    uint64_t size,
    void* data
);

/** code
 * Create a `mc_Program_t` object.
 *
 * - `device`: A reference to a `mc_Device_t` object
 * - `codeSize`: The size of the shader code
 * - `code`: The shader code
 * - `entryPoint`: The entry point name, generally `"main"`
 * - returns: A program object on success, `NULL` on error
 */
mc_Program_t* mc_program_create(
    mc_Device_t* device,
    size_t codeSize,
    const char* code,
    const char* entryPoint
);

/** code
 * Create a `mc_Program_t` object from a file.
 *
 * - `device`: A reference to a `mc_Device_t` object
 * - `fileName`: The path to the shader code
 * - `entryPoint`: The entry point name, generally `"main"`
 * - returns: A program object on success, `NULL` on error
 */
mc_Program_t* mc_program_create_from_file(
    mc_Device_t* device,
    const char* fileName,
    const char* entryPoint
);

/** code
 * Destroy a `mc_Program_t` object.
 *
 * - `self`: A reference to a `mc_Program_t` object
 */
void mc_program_destroy(mc_Program_t* self);

/** code
 * Bind buffers to a `mc_Program_t` object.
 *
 * - `self`: A reference to a `mc_Program_t` object
 * - `dimX`: The number of local workgroups in the x dimension
 * - `dimY`: The number of local workgroups in the y dimension
 * - `dimZ`: The number of local workgroups in the z dimension
 * - `...`: A list of buffers to bind to the program
 * - returns: Time taken to run the program, in seconds, or `-1.0` on error
 */
#define mc_program_run(self, dimX, dimY, dimZ, ...)                            \
    mc_program_run__(self, dimX, dimY, dimZ, ##__VA_ARGS__, NULL)

/** code
 * Get the current time.
 *
 * - returns: The current time, in seconds
 */
double mc_get_time();

/** code
 * Convert a `mc_LogLevel_t` enum to a human readable string.
 *
 * - `level`: A `mc_LogLevel_t` value
 * - returns: A human readable string (`NULL` terminated)
 */
const char* mc_log_level_to_str(mc_LogLevel_t level);

/** code
 * Convert a `mc_DeviceType_t` enum to a human readable string.
 *
 * - `type`: A `mc_DeviceType_t` value
 * - returns: A human readable string (`NULL` terminated)
 */
const char* mc_device_type_to_str(mc_DeviceType_t type);

/** code
 * Read the contents of a file into a buffer.
 *
 * - `path`: The path to the file
 * - `contents`: A buffer to read the file into, set to `NULL` to just get the
 *   size
 * - returns: The size of the file, in bytes
 */
size_t mc_read_file(const char* path, char* contents);

/** code
 * Default callback function that can be passed to `mc_instance_create()`. Just
 * prints all messages to stdout. Use as a template for your own callback.
 *
 * See `mc_log_cb` for more info about the arguments.
 */
void mc_default_log_cb( //
    mc_LogLevel_t lvl,
    const char* src,
    void* arg,
    char const* file,
    int line,
    const char* fmt,
    ...
);

/** code
 * Wrapped functions. Don't use these directly, use their corresponding macros.
 */
double mc_program_run__(
    mc_Program_t* self,
    uint32_t dimX,
    uint32_t dimY,
    uint32_t dimZ,
    ...
);

#endif // MC_H_INCLUDE_GUARD

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