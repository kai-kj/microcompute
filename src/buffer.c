#include "microcompute_internal.h"

mc_Buffer* mc_buffer_create(mc_BufferType type, uint64_t size) {
    mc_Buffer* buffer = malloc(sizeof *buffer);
    buffer->type = type == MC_BUFFER_UNIFORM ? GL_UNIFORM_BUFFER
                                             : GL_SHADER_STORAGE_BUFFER;
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