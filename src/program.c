#include "_microcompute.h"

mc_Result check_shader(GLuint shader, uint32_t maxLen, char* err) {
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

mc_Result check_program(GLuint program, uint32_t maxLen, char* err) {
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

mc_Result mc_program_from_str(
    mc_Program* program,
    const char* code,
    uint32_t maxErrorLength,
    char* error
) {
    program->shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(program->shader, 1, &code, NULL);
    glCompileShader(program->shader);

    mc_Result res = check_shader(program->shader, maxErrorLength, error);
    if (!res.ok) return res;

    program->program = glCreateProgram();
    glAttachShader(program->program, program->shader);
    glLinkProgram(program->program);

    res = check_program(program->program, maxErrorLength, error);
    if (!res.ok) return res;

    return GL_CHECK_ERROR();
}

mc_Result mc_program_from_file(
    mc_Program* program,
    const char* path,
    uint32_t maxErrorLength,
    char* error
) {
    uint32_t fileSize;
    ASSERT(mc_read_file(&fileSize, NULL, path).ok, "failed to read file");
    char code[fileSize];
    ASSERT(mc_read_file(&fileSize, code, path).ok, "failed to read file");

    return mc_program_from_str(program, code, maxErrorLength, error);
}

mc_Result mc_program_destroy(mc_Program* program) {
    ASSERT(program != NULL, "`program` is NULL");
    if (program->shader != 0) glDeleteShader(program->shader);
    if (program->program != 0) glDeleteProgram(program->program);
    return GL_CHECK_ERROR();
}

mc_Result mc_program_dispatch(mc_Program* program, mc_ivec3 size) {
    glUseProgram(program->program);
    glDispatchCompute(size.x, size.y, size.z);
    return GL_CHECK_ERROR();
}