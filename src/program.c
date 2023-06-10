#include "microcompute_internal.h"

static char* get_shader_errors(GLuint shader) {
    int32_t success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success) return NULL;

    GLint len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    char* error = malloc(len);
    glGetShaderInfoLog(shader, len, NULL, error);

    return error;
}

static char* get_program_errors(GLuint program) {
    int32_t success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success) return NULL;

    GLint len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    char* error = malloc(len);
    glGetProgramInfoLog(program, len, NULL, error);

    return error;
}

static char* alloc_string(char* string) {
    size_t len = strlen(string);
    char* error = malloc(len + 1);
    memcpy(error, string, len);
    string[len] = '\0';
    return error;
}

mc_Program* mc_program_create(const char* code) {
    mc_Program* program = malloc(sizeof *program);
    program->error = NULL;

    program->shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(program->shader, 1, &code, NULL);
    glCompileShader(program->shader);

    if ((program->error = get_shader_errors(program->shader))) return program;

    program->program = glCreateProgram();
    glAttachShader(program->program, program->shader);
    glLinkProgram(program->program);

    if ((program->error = get_program_errors(program->program))) {
        glDeleteShader(program->shader);
        return program;
    }

    return program;
}

void mc_program_destroy(mc_Program* program) {
    if (!program) return;
    if (program->shader) glDeleteShader(program->shader);
    if (program->program) glDeleteProgram(program->program);
    if (program->error) free(program->error);
    free(program);
}

char* mc_program_check(mc_Program* program) {
    return program->error;
}

void mc_program_run(
    mc_Program* program,
    mc_uvec3 workgroup_size,
    mc_Buffer** buffers
) {
    if (program->error) return;
    glUseProgram(program->program);
    for (uint32_t i = 0; buffers[i] != NULL; i++)
        glBindBufferBase(buffers[i]->type, i, buffers[i]->buffer);
    glDispatchCompute(workgroup_size.x, workgroup_size.y, workgroup_size.z);
}

double mc_program_run_timed(
    mc_Program* program,
    mc_uvec3 workgroup_size,
    mc_Buffer** buffers
) {
    double startTime = mc_get_time();
    mc_program_run(program, workgroup_size, buffers);
    mc_wait();
    return mc_get_time() - startTime;
}