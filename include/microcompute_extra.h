#ifndef MICROCOMPUTE_EXTRA_H
#define MICROCOMPUTE_EXTRA_H

#include <stdint.h>

/**
 * A device.
 */
typedef struct mc_Device mc_Device;

/**
 * The type of a buffer.
 */
typedef enum mc_BufferType mc_BufferType;

/**
 * A buffer.
 */
typedef struct mc_Buffer mc_Buffer;

/**
 * A program.
 */
typedef struct mc_Program mc_Program;

/**
 * A hybrid buffer. This buffer is can be accessed from the CPU while still
 * being fast to access from the GPU.
 */
typedef struct mce_HBuffer mce_HBuffer;

/**
 * Create an empty hybrid buffer.
 * @param device A device
 * @param size The size of the buffer
 * @return An new hybrid buffer on success, `NULL` on error
 */
mce_HBuffer* mce_hybrid_buffer_create(mc_Device* device, uint64_t size);

/**
 * Create an hybrid buffer from some data.
 * @param device A device
 * @param size The size of the buffer
 * @param data The data to put in the buffer
 * @return An new hybrid buffer on success, `NULL` on error
 */
mce_HBuffer* mce_hybrid_buffer_create_from(
    mc_Device* device,
    uint64_t size,
    void* data
);

/**
 * Destroy a hybrid buffer.
 * @param hBuffer A hybrid buffer
 */
void mce_hybrid_buffer_destroy(mce_HBuffer* hBuffer);

/**
 * Get the size of a hybrid buffer.
 * @param hBuffer A hybrid buffer
 * @return The size of the hybrid buffer
 */
uint64_t mce_hybrid_buffer_get_size(mce_HBuffer* hBuffer);

/**
 * Write data to a hybrid buffer.
 * @param hBuffer A hybrid buffer
 * @param offset The offset from witch to start writing the data, in bytes
 * @param size The size of the data to write, in bytes
 * @param data A reference to the data to write
 * @return The number of bytes written, 0 on error
 */
uint64_t mce_hybrid_buffer_write(
    mce_HBuffer* hBuffer,
    uint64_t offset,
    uint64_t size,
    void* data
);

/**
 * Read data from a hybrid buffer.
 * @param hBuffer A hybrid buffer
 * @param offset The offset from witch to start reading the data, in bytes
 * @param size The size of the data to read, in bytes
 * @param data A reference to the buffer to read the data into
 * @return The number of bytes read, 0 on error
 */
uint64_t mce_hybrid_buffer_read(
    mce_HBuffer* hBuffer,
    uint64_t offset,
    uint64_t size,
    void* data
);

/**
 * Create an buffer from some data.
 * @param device A device
 * @param type The type of the buffer
 * @param size The size of the buffer
 * @param data The data to put in the buffer
 * @return An new buffer on success, `NULL` on error
 */
mc_Buffer* mce_buffer_create_from(
    mc_Device* device,
    mc_BufferType type,
    uint64_t size,
    void* data
);

/**
 * Reallocate a buffer. If the buffer is of type `MC_BUFFER_TYPE_CPU`, the data
 * will be copied.
 *
 * @param buffer A buffer
 * @param size The new size of the buffer
 * @return A new buffer on success, `NULL` on error
 */
mc_Buffer* mce_buffer_realloc(mc_Buffer* buffer, uint64_t size);

/**
 * Reallocate a hybrid buffer. This will copy the data from the old buffer.
 * @param hBuffer A buffer
 * @param size The new size of the buffer
 * @return A new buffer on success, `NULL` on error
 */
mce_HBuffer* mce_hybrid_buffer_realloc(mce_HBuffer* hBuffer, uint64_t size);

/**
 * Create a program from some SPIRV code.
 * @param device A device
 * @param filename The name of the file containing the SPIRV code
 * @param entryPoint The entry point name, generally `"main"`
 * @return A new program on success, `NULL` on error
 */
mc_Program* mce_program_create_from_file(
    mc_Device* device,
    const char* filename,
    const char* entryPoint
);

#endif // MICROCOMPUTE_EXTRA_H