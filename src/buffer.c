#include "_microcompute.h"

mc_Buffer *mc_buffer_create(int binding, size_t size) {
	mc_Buffer *buffer = malloc(sizeof(mc_Buffer));
	glGenBuffers(1, &buffer->ssbo);
	mc_buffer_rebind(buffer, binding);
	mc_buffer_resize(buffer, size);
	return buffer;
}

void mc_buffer_destroy(mc_Buffer *buffer) {
	if (buffer == NULL) return;
	if (buffer->ssbo != 0) glDeleteBuffers(1, &buffer->ssbo);
	free(buffer);
}

void mc_buffer_rebind(mc_Buffer *buffer, int binding) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer->ssbo);
}

void mc_buffer_resize(mc_Buffer *buffer, size_t size) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_COPY);
}

size_t mc_buffer_get_size(mc_Buffer *buffer) {
	GLint size;
	glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &size);
	return size;
}

size_t mc_buffer_write(mc_Buffer *buffer, size_t off, size_t size, void *data) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);

	if (off + size > mc_buffer_get_size(buffer)) {
		debug_msg(
			mc_DebugLevel_MEDIUM,
			"offset %d, size %d is larger than buffer size %d",
			off,
			size,
			mc_buffer_get_size(buffer)
		);
		return 0;
	}

	glBufferSubData(GL_SHADER_STORAGE_BUFFER, off, size, data);
	return size;
}

size_t mc_buffer_read(mc_Buffer *buffer, size_t off, size_t size, void *data) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->ssbo);

	if (off + size > mc_buffer_get_size(buffer)) {
		debug_msg(
			mc_DebugLevel_MEDIUM,
			"offset %d, size %d is larger than buffer size %d",
			off,
			size,
			mc_buffer_get_size(buffer)
		);
		return 0;
	}

	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, off, size, data);
	return size;
}