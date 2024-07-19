#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "device.h"
#include "hybrid_buffer.h"
#include "log.h"
#include "microcompute_extra.h"

mc_Buffer* mc_buffer_create_from(
    mc_Device* device,
    mc_BufferType type,
    uint64_t size,
    void* data
) {
    mc_Buffer* buffer = mc_buffer_create(device, type, size);
    if (!buffer) return NULL;
    mc_buffer_write(buffer, 0, size, data);
    return buffer;
}

mc_Buffer* mc_buffer_realloc(mc_Buffer* buffer, uint64_t size) {
    if (!buffer) return NULL;
    DEBUG(buffer, "reallocating buffer: %ld -> %ld", buffer->size, size);

    mc_Buffer* new = mc_buffer_create(buffer->device, buffer->type, size);
    if (!new) return NULL;

    if (buffer->type == MC_BUFFER_TYPE_CPU) {
        uint64_t minSize = size < buffer->size ? size : buffer->size;
        mc_buffer_write(new, 0, minSize, buffer->map);
    } else {
        WARN(buffer, "buffer cannot be written to, the data will be lost");
    }

    mc_buffer_destroy(buffer);
    return new;
}

mc_HBuffer* mc_hybrid_buffer_realloc(mc_HBuffer* old, uint64_t size) {
    if (!old) return NULL;
    DEBUG(
        old,
        "reallocating hybrid buffer: %ld -> %ld",
        old->gpuBuff.size,
        size
    );

    mc_HBuffer* new = mc_hybrid_buffer_create(old->gpuBuff.device, size);
    if (!new) return NULL;

    uint64_t minSize = size < old->gpuBuff.size ? size : old->gpuBuff.size;
    mc_buffer_copier_copy(
        old->copier,
        &new->gpuBuff,
        &old->gpuBuff,
        0,
        0,
        minSize
    );

    mc_hybrid_buffer_destroy(old);
    return new;
}

char* read_file(const char* filename, size_t* size) {
    FILE* fp = fopen(filename, "rb");

    fseek(fp, 0, SEEK_END);
    size_t dataSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* data = malloc(dataSize + 1);
    fread(data, 1, dataSize, fp);
    fclose(fp);

    data[dataSize] = '\0';

    if (size) *size = dataSize;

    return data;
}