#include "_microcompute.h"

static mc_Result check_shader(GLuint shader, uint32_t maxLen, char* err) {
    int32_t success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success) return GL_CHECK_ERROR();

    int32_t length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    ASSERT(
        (uint32_t)length < maxLen,
        "error message longer than max error buffer size"
    );
    glGetShaderInfoLog(shader, length, NULL, err);
    return ERROR("shader compile error");
}

static mc_Result check_program(GLuint program, uint32_t maxLen, char* err) {
    int32_t success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success) return GL_CHECK_ERROR();

    int32_t length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    ASSERT(
        (uint32_t)length < maxLen,
        "error message longer than max error buffer size"
    );
    glGetProgramInfoLog(program, length, NULL, err);
    return ERROR("program link error");
}

mc_Program* mc_program_create_from_string(
    const char* code,
    uint32_t maxErrorLength,
    char* error
) {
    mc_Program* program = malloc(sizeof(*program));

    program->shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(program->shader, 1, &code, NULL);
    glCompileShader(program->shader);

    mc_Result res = check_shader(program->shader, maxErrorLength, error);
    if (!res.ok) {
        free(program);
        return NULL;
    };

    program->program = glCreateProgram();
    glAttachShader(program->program, program->shader);
    glLinkProgram(program->program);

    res = check_program(program->program, maxErrorLength, error);
    if (!res.ok) {
        free(program);
        return NULL;
    };

    return program;
}

mc_Program* mc_program_create_from_file(
    const char* path,
    uint32_t maxErrorLength,
    char* error
) {
    char* code = mc_read_file(path, NULL);
    if (code == NULL) return NULL;

    mc_Program* program
        = mc_program_create_from_string(code, maxErrorLength, error);
    free(code);
    return program;
}

mc_Result mc_program_destroy(mc_Program* program) {
    ASSERT(program != NULL, "`program` is NULL");
    if (program->shader != 0) glDeleteShader(program->shader);
    if (program->program != 0) glDeleteProgram(program->program);
    return GL_CHECK_ERROR();
}

mc_Result mc_program_dispatch(
    mc_Program* program,
    mc_ivec3 size,
    uint32_t bufferCount,
    mc_Buffer** buffers
) {
    for (uint32_t i = 0; i < bufferCount; i++) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[i]->ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, buffers[i]->ssbo);
    }

    glUseProgram(program->program);
    glDispatchCompute(size.x, size.y, size.z);
    glMemoryBarrier(program->program);

    return GL_CHECK_ERROR();
}