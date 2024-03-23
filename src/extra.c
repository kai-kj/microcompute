#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "device.h"
#include "log.h"
#include "microcompute_extra.h"

mc_Buffer* mce_buffer_create_from(
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

mc_Buffer* mce_buffer_realloc(mc_Buffer* buffer, uint64_t size) {
    if (!buffer) return NULL;
    DEBUG(buffer, "reallocating buffer: %ld -> %ld", buffer->size, size);

    mc_Buffer* new = mc_buffer_create(buffer->device, buffer->type, size);
    if (!new) return NULL;

    uint64_t minSize = size < buffer->size ? size : buffer->size;
    mc_buffer_write(new, 0, minSize, buffer->map);
    mc_buffer_destroy(buffer);
    return new;
}

mc_Program* mce_program_create_from_file(
    mc_Device* device,
    const char* filename,
    const char* entryPoint
) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        ERROR(device, "could not open file %s", filename);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    size_t codeLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* code = malloc(codeLen);
    fread(code, 1, codeLen, fp);
    fclose(fp);

    mc_Program* program = mc_program_create(device, codeLen, code, entryPoint);
    free(code);

    return program;
}