#ifndef MC_H_INCLUDE_GUARD
#define MC_H_INCLUDE_GUARD

/**
 * @file microcompute.h
 * This library contains utilities that can be used to easily run SPIR-V compute
 * shaders using vulkan.
 */

#include <stdbool.h>
#include <stdint.h>

/**
 * The severity of a log message
 */
typedef enum mc_LogLevel {
    MC_LOG_LEVEL_DEBUG,   ///< Debug information logs
    MC_LOG_LEVEL_INFO,    ///< Information logs
    MC_LOG_LEVEL_WARN,    ///< Warning logs
    MC_LOG_LEVEL_ERROR,   ///< Error logs
    MC_LOG_LEVEL_UNKNOWN, ///< Unknown logs
} mc_LogLevel;

/**
 * The type of a device.
 */
typedef enum mc_DeviceType {
    MC_DEVICE_TYPE_DGPU,  ///< discrete GPU
    MC_DEVICE_TYPE_IGPU,  ///< integrated GPU
    MC_DEVICE_TYPE_VGPU,  ///< virtual GPU
    MC_DEVICE_TYPE_CPU,   ///< CPU
    MC_DEVICE_TYPE_OTHER, ///< other (or unknown)
} mc_DeviceType;

/**
 * The type of a buffer.
 */
typedef enum mc_BufferType {
    MC_BUFFER_TYPE_CPU, ///< Accessible from CPU, but slow GPU access
    MC_BUFFER_TYPE_GPU, ///< Not accessible from CPU, but fast GPU access
} mc_BufferType;

/**
 * Options to pass to mc_program_code_create_*.
 */
typedef struct mc_CompileDefinition {
    char* key;   ///< The key/name
    char* value; ///< The value
} mc_CompileDefinition;

/**
 * The log callback type.
 * @param arg The value passed to `logArg` in `mc_instance_create()`
 * @param lvl A `mc_LogLevel` indicating the severity of the message
 * @param src The source of the message
 * @param file The file the message originated from
 * @param line The line the message originated from
 * @param fmt The message formatting (same as `printf()`)
 * @param ... The arguments for the formatting
 */
typedef void(mc_log_fn)( //
    void* arg,
    mc_LogLevel lvl,
    const char* src,
    const char* file,
    int line,
    const char* fmt,
    ...
);

/**
 * An instance of the library.
 */
typedef struct mc_Instance mc_Instance;

/**
 * A device.
 */
typedef struct mc_Device mc_Device;

/**
 * A buffer.
 */
typedef struct mc_Buffer mc_Buffer;

/**
 * A buffer copier.
 */
typedef struct mc_BufferCopier mc_BufferCopier;

/**
 * Code that can be used to create an mc_Program.
 */
typedef struct mc_ProgramCode mc_ProgramCode;

/**
 * A program.
 */
typedef struct mc_Program mc_Program;

/**
 * Create an instance of the library. If `log_fn` is `NULL`, no logs will be
 * from microcompute.
 *
 * @param log_fn A function to call when there is a message from the library
 * @param logArg A value to pass to the `arg` parameter of `log_fn`
 * @return A new instance success, `NULL` on error
 */
mc_Instance* mc_instance_create(mc_log_fn* log_fn, void* logArg);

/**
 * Destroy an instance of the library.
 * @param instance An instance of the library
 */
void mc_instance_destroy(mc_Instance* instance);

/**
 * Get the number of available devices.
 * @param instance An instance of the library
 * @return The number of devices
 */
uint32_t mc_instance_get_device_count(mc_Instance* instance);

/**
 * Get the devices available to an instance.
 * @param instance A n instance of the library
 * @return An array of devices
 */
mc_Device** mc_instance_get_devices(mc_Instance* instance);

/**
 * Get the type of a device.
 * @param device A device
 * @return The type of the device
 */
mc_DeviceType mc_device_get_type(mc_Device* device);

/**
 * Get the max total workgroup count of a device.
 * @param device A device
 * @return The max total workgroup count
 */
uint32_t mc_device_get_max_workgroup_size_total(mc_Device* device);

/**
 * Get the max workgroup size (for each x, y, z) of a device.
 * @param device A device
 * @return The max workgroup size, as a 3 element array
 */
uint32_t* mc_device_get_max_workgroup_size_shape(mc_Device* device);

/**
 * Get the max workgroup count (for each x, y, z) of a device.
 * @param device A device
 * @return The max workgroup count, as a 3 element array
 */
uint32_t* mc_device_get_max_workgroup_count(mc_Device* device);

/**
 * Get the name of a device.
 * @param device A device
 * @return The name of the device
 */
char* mc_device_get_name(mc_Device* device);

/**
 * Create an empty buffer.
 * @param device A device
 * @param type The type of the buffer
 * @param size The size of the buffer
 * @return An new buffer on success, `NULL` on error
 */
mc_Buffer* mc_buffer_create(
    mc_Device* device,
    mc_BufferType type,
    uint64_t size
);

/**
 * Destroy a buffer.
 * @param buffer A buffer
 */
void mc_buffer_destroy(mc_Buffer* buffer);

/**
 * Get the size of a buffer.
 * @param buffer A buffer
 * @return The size of the buffer
 */
uint64_t mc_buffer_get_size(mc_Buffer* buffer);

/**
 * Write data to a buffer. Must be of type `MC_BUFFER_TYPE_CPU`.
 * @param buffer A buffer
 * @param offset The offset from witch to start writing the data, in bytes
 * @param size The size of the data to write, in bytes
 * @param data A reference to the data to write
 * @return The number of bytes written, 0 on error
 */
uint64_t mc_buffer_write(
    mc_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
);

/**
 * Read data from a buffer. Must be of type `MC_BUFFER_TYPE_CPU`.
 * @param buffer A buffer
 * @param offset The offset from witch to start reading the data, in bytes
 * @param size The size of the data to read, in bytes
 * @param data A reference to the buffer to read the data into
 * @return The number of bytes read, 0 on error
 */
uint64_t mc_buffer_read(
    mc_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
);

/**
 * Create a buffer copier.
 * @param device A device
 * @return A new buffer copier on success, `NULL` on error
 */
mc_BufferCopier* mc_buffer_copier_create(mc_Device* device);

/**
 * Destroy a buffer copier.
 * @param copier A buffer copier
 */
void mc_buffer_copier_destroy(mc_BufferCopier* copier);

/**
 * Copy data from one buffer to another.
 * @param copier A buffer copier
 * @param src The source buffer
 * @param dst The destination buffer
 * @param srcOffset The offset in the source buffer to start copying from
 * @param dstOffset The offset in the destination buffer to start copying to
 * @param size The number of bytes to copy
 * @return The number of bytes copied, 0 on error
 */
uint64_t mc_buffer_copier_copy(
    mc_BufferCopier* copier,
    mc_Buffer* src,
    mc_Buffer* dst,
    uint64_t srcOffset,
    uint64_t dstOffset,
    uint64_t size
);

/**
 * Create some program code from SPIR-V code.
 * @param instance A instance
 * @param size The size of the code
 * @param code The code contens, copied internaly
 * @return  New program code on success, `NULL` on error
 */
mc_ProgramCode* mc_program_code_create_from_spirv(
    mc_Instance* instance,
    size_t size,
    const char* code
);

/**
 * Create some program code from GLSL code.
 * @param instance A instance
 * @param name The name of the code (used in compile error messages)
 * @param code The code contens, copied internaly
 * @param entry The entry point (the name of the "main" function
 * @param ... Any compile-time definitions (#define's)
 * @return  New program code on success, `NULL` on error
 */
#define mc_program_code_create_from_glsl(instance, name, code, entry, ...)     \
    mc_program_code_create_from_glsl__(                                        \
        instance,                                                              \
        name,                                                                  \
        code,                                                                  \
        entry,                                                                 \
        ##__VA_ARGS__,                                                         \
        (mc_CompileDefinition){NULL, NULL}                                     \
    )

void mc_program_code_destroy(mc_ProgramCode* programCode);

/**
 * Create a program from some SPIRV code.
 * @param device A device
 * @param code The shader code
 * @return A new program on success, `NULL` on error
 */
mc_Program* mc_program_create(mc_Device* device, mc_ProgramCode* code);

/**
 * Destroy a program.
 * @param program A program
 */
void mc_program_destroy(mc_Program* program);

/**
 * Run a program.
 * @param program A program
 * @param dimX The number of workgroups to run in the x direction
 * @param dimY The number of workgroups to run in the y direction
 * @param dimZ The number of workgroups to run in the z direction
 * @param ... Buffers / hybrid buffers to pass to the program
 * @return The time taken to run the program, in seconds
 */
#define mc_program_run(program, dimX, dimY, dimZ, ...)                         \
    mc_program_run__(program, dimX, dimY, dimZ, ##__VA_ARGS__, NULL)

/**
 * Get the current time.
 * @return The current time in seconds
 */
double mc_get_time();

/**
 * Convert a `mc_LogLevel` enum to a human readable string.
 * @param level A log level
 * @return A human readable string (`NULL` terminated)
 */
const char* mc_log_level_to_str(mc_LogLevel level);

/**
 * Convert a `mc_DeviceType` enum to a human readable string.
 * @param type A device type
 * @return A human readable string (`NULL` terminated)
 */
const char* mc_device_type_to_str(mc_DeviceType type);

/**
 * A simple log callback that prints to `stderr`. Use this as a base for your
 * own log callback.
 */
void mc_log_cb_simple(
    void* arg,
    mc_LogLevel lvl,
    const char* src,
    char const* file,
    int line,
    const char* fmt,
    ...
);

/**
 * For internal use
 */
mc_ProgramCode* mc_program_code_create_from_glsl__(
    mc_Instance* instance,
    const char* name,
    const char* code,
    const char* entry,
    ...
);

/**
 * For internal use
 */
double mc_program_run__(
    mc_Program* program,
    uint32_t dimX,
    uint32_t dimY,
    uint32_t dimZ,
    ...
);

#endif // MC_H_INCLUDE_GUARD
