#pragma once

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef STANDALONE
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <fcntl.h>
#include <gbm.h>
#include <unistd.h>
#endif

#include "microcompute.h"

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

#define OK                                                                     \
    ((mc_Result){                                                              \
        .ok = MC_TRUE,                                                         \
        .file = __FILE__,                                                      \
        .line = __LINE__,                                                      \
        .func = __FUNCTION__,                                                  \
        .message = (char*){"no errors here :)"}})

#define ERROR(msg)                                                             \
    ((mc_Result){                                                              \
        .ok = MC_FALSE,                                                        \
        .file = __FILE__,                                                      \
        .line = __LINE__,                                                      \
        .func = __FUNCTION__,                                                  \
        .message = (char*){(msg)}})

#define ASSERT(cond, msg)                                                      \
    do {                                                                       \
        if (!(cond)) return ERROR((msg));                                      \
    } while (0)
