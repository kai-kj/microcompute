#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/glew.h>
#include <fcntl.h>
#include <gbm.h>
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