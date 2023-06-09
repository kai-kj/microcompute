#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "glad/glad.h"
#include "glad/glad_egl.h"
#include "microcompute.h"

typedef struct mc_Program {
    GLint shader;
    GLint program;
    char* error;
} mc_Program;

typedef struct mc_Buffer {
    GLuint buffer;
    GLenum type;
} mc_Buffer;