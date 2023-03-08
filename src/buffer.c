#include "_microcompute.h"

mc_Buffer* mc_buffer_create(uint64_t size) {
    mc_Buffer* buffer = malloc(sizeof(*buffer));
    glGenBuffers(1, &buffer->ssbo);
    mc_buffer_resize(buffer, size);
    return buffer;
}

mc_Result mc_buffer_destroy(mc_Buffer* buffer) {
    MC_ASSERT(buffer != NULL, "buffer is NULL");
    if (buffer->ssbo != 0) glDeleteBuffers(1, &buffer->ssbo);
    return GL_CHECK_ERROR();
}

mc_Result mc_buffer_resize(mc_Buffer* buffer, uint64_t size) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_COPY);
    return GL_CHECK_ERROR();
}

mc_Result mc_buffer_get_size(mc_Buffer* buffer, uint64_t* size) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);
    GLint iSize;
    glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &iSize);
    *size = iSize;
    return GL_CHECK_ERROR();
}

mc_Result mc_buffer_write(
    mc_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);

    uint64_t buffSize;
    mc_buffer_get_size(buffer, &buffSize);

    MC_ASSERT(
        offset + size <= buffSize,
        "offset + size larger than buffer size"
    );

    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    return GL_CHECK_ERROR();
}

mc_Result mc_buffer_read(
    mc_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);

    uint64_t buffSize;
    mc_buffer_get_size(buffer, &buffSize);

    MC_ASSERT(
        offset + size <= buffSize,
        "offset + size larger than buffer size"
    );

    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    return GL_CHECK_ERROR();
}