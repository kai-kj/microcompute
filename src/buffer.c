#include "_microcompute.h"

mc_Buffer *mc_buffer_create(int index) {
	mc_Buffer *buffer = malloc(sizeof(mc_Buffer));
	glGenBuffers(1, &buffer->ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffer->ssbo);
	return buffer;
}

void mc_buffer_destroy(mc_Buffer *buffer) {
	if (buffer == NULL) return;
	if (buffer->ssbo != 0) glDeleteBuffers(1, &buffer->ssbo);
	free(buffer);
}

void mc_buffer_write(mc_Buffer *buffer, size_t size, void *data) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_COPY);
}

void mc_buffer_modify(mc_Buffer *buffer, size_t off, size_t size, void *data) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);

	GLint ssboSize;
	glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &ssboSize);
	if (off + size > (size_t)ssboSize) {
		debug_msg(
			mc_DebugLevel_MEDIUM,
			"offset %d, size %d is larger than buffer ssboSize %d",
			off,
			size,
			ssboSize
		);
		return;
	}

	glBufferSubData(GL_SHADER_STORAGE_BUFFER, off, size, data);
}

void mc_buffer_read(mc_Buffer *buffer, size_t off, size_t size, void *data) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);

	GLint ssboSize;
	glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &ssboSize);
	if (off + size > (size_t)ssboSize) {
		debug_msg(
			mc_DebugLevel_MEDIUM,
			"offset %d, size %d is larger than buffer ssboSize %d",
			off,
			size,
			ssboSize
		);
		return;
	}

	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, off, size, data);
}