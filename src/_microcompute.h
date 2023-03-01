#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/glew.h>
#include <fcntl.h>
#include <gbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "microcompute.h"

typedef struct mc_Program {
    int32_t shader;
    int32_t program;
} mc_Program;

typedef struct mc_Buffer {
    uint32_t ssbo;
} mc_Buffer;

#define GL_CHECK_ERROR() gl_check_error(__LINE__, __FILE__, __FUNCTION__)

mc_Result gl_check_error(uint32_t line, const char* file, const char* func);
mc_Result mc_read_file(uint32_t* size, char* contents, const char* path);

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
