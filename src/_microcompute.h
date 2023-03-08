#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"
#include "microcompute.h"

#ifdef MC_STANDALONE_MODE
#include "glad/glad_egl.h"
#endif

typedef struct mc_Program {
    GLint shader;
    GLint program;
} mc_Program;

typedef struct mc_Buffer {
    GLuint ssbo;
} mc_Buffer;

#define GL_CHECK_ERROR() gl_check_error(__LINE__, __FILE__, __FUNCTION__)

mc_Result gl_check_error(uint32_t line, const char* file, const char* func);
char* mc_read_file(const char* path, uint32_t* size);

#define MC_OK                                                                  \
    ((mc_Result){                                                              \
        .ok = MC_TRUE,                                                         \
        .file = __FILE__,                                                      \
        .line = __LINE__,                                                      \
        .func = __FUNCTION__,                                                  \
        .message = (char*){"no errors here :)"}})

#define MC_ERROR(msg)                                                          \
    ((mc_Result){                                                              \
        .ok = MC_FALSE,                                                        \
        .file = __FILE__,                                                      \
        .line = __LINE__,                                                      \
        .func = __FUNCTION__,                                                  \
        .message = (char*){(msg)}})

#define MC_ASSERT(cond, msg)                                                   \
    do {                                                                       \
        if (!(cond)) return MC_ERROR((msg));                                   \
    } while (0)
