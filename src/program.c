#include "_microcompute.h"

static char *read_file(const char *filePath) {
	FILE *fp = fopen(filePath, "rb");
	if (fp == NULL) return NULL;

	fseek(fp, 0, SEEK_END);
	long length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *buff = malloc(length);
	fread(buff, 1, length, fp);
	buff[length] = '\0';
	fclose(fp);

	return buff;
}

static char *get_shader_compile_errors(GLuint shader) {
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success) return NULL;

	int length;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
	char *buff = malloc(length + 1);
	glGetShaderInfoLog(shader, length, NULL, buff);
	buff[length] = '\0';
	return buff;
}

static char *get_program_link_errors(GLuint program) {
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (success) return NULL;

	int length;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
	char *buff = malloc(length + 1);
	glGetProgramInfoLog(program, length, NULL, buff);
	buff[length] = '\0';
	return buff;
}

mc_Program *mc_program_from_str(const char *programCode) {
	mc_Program *program = malloc(sizeof(mc_Program));

	program->shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(program->shader, 1, &programCode, NULL);
	glCompileShader(program->shader);

	char *buff = get_shader_compile_errors(program->shader);
	if (buff != NULL) {
		debug_msg(mc_DebugLevel_MEDIUM, "compile error:\n%s", buff);
		mc_program_destroy(program);
		return NULL;
	}

	program->program = glCreateProgram();
	glAttachShader(program->program, program->shader);
	glLinkProgram(program->program);

	buff = get_program_link_errors(program->program);
	if (buff != NULL) {
		debug_msg(mc_DebugLevel_MEDIUM, "link error:\n%s", buff);
		mc_program_destroy(program);
		return NULL;
	}

	return program;
}

mc_Program *mc_program_from_file(const char *programPath) {
	char *shaderCode = read_file(programPath);
	if (shaderCode == NULL) {
		debug_msg(mc_DebugLevel_MEDIUM, "failed to open %s", programPath);
		return NULL;
	}
	mc_Program *shader = mc_program_from_str(shaderCode);
	free(shaderCode);
	return shader;
}

void mc_program_destroy(mc_Program *program) {
	if (program != NULL) {
		if (program->shader != 0) glDeleteShader(program->shader);
		if (program->program != 0) glDeleteProgram(program->program);
		free(program);
	}
}

void mc_program_dispatch(mc_Program *program, mc_ivec3 size) {
	glUseProgram(program->program);
	glDispatchCompute(size.x, size.y, size.z);
}