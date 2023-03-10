#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"
#include "glad/glad_egl.h"
#include "microcompute.h"

typedef struct mc_Program {
    GLint shader;
    GLint program;
} mc_Program;

typedef struct mc_Buffer {
    GLuint ssbo;
} mc_Buffer;

#define GL_CHECK_ERROR() gl_check_error(__LINE__, __FILE__, __FUNCTION__)

k_Result gl_check_error(uint32_t line, const char* file, const char* func);
char* mc_read_file(const char* path, uint32_t* size);