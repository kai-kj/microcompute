#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/glew.h>
#include <fcntl.h>
#include <gbm.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "micro_compute.h"

typedef struct mc_Program {
	GLuint shader;
	GLuint program;
} mc_Program;

typedef struct mc_Buffer {
	GLuint ssbo;
} mc_Buffer;

#define debug_msg(level, ...) debug_msg_fn(__FUNCTION__, level, __VA_ARGS__)

void debug_msg_fn(const char *function, int level, char *format, ...);