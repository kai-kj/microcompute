#include "_micro_compute.h"

static char *_read_file(const char *filePath) {
	FILE *fp = fopen(filePath, "rb");
	if (fp == NULL) return NULL;

	fseek(fp, 0, SEEK_END);
	long length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *buff = malloc(length);
	fread(buff, 1, length, fp);
	fclose(fp);

	return buff;
}

static char *_get_shader_compile_errors(GLuint shader) {
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

static char *_get_program_link_errors(GLuint program) {
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

mc_Program *mc_program_create_from_str(const char *programCode) {
	mc_Program *program = malloc(sizeof(mc_Program));

	program->shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(program->shader, 1, &programCode, NULL);
	glCompileShader(program->shader);

	char *buff = _get_shader_compile_errors(program->shader);
	if (buff != NULL) {
		printf("compile error:\n%s", buff);
		mc_program_destroy(program);
		return NULL;
	}

	program->program = glCreateProgram();
	glAttachShader(program->program, program->shader);
	glLinkProgram(program->program);

	buff = _get_program_link_errors(program->program);
	if (buff != NULL) {
		printf("link error:\n%s", buff);
		mc_program_destroy(program);
		return NULL;
	}

	return program;
}

mc_Program *mc_program_create_from_file(const char *programPath) {
	char *shaderCode = _read_file(programPath);
	if (shaderCode == NULL) return NULL;
	mc_Program *shader = mc_program_create_from_str(shaderCode);
	free(shaderCode);
	return shader;
}