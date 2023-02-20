#include "_micro_compute.h"

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

void mc_program_set_float(mc_Program *program, char *name, float value) {
	GLint loc = glGetUniformLocation(program->program, name);
	if (loc == -1) return;
	glUseProgram(program->program);
	glUniform1f(loc, value);
}

void mc_program_set_vec2(mc_Program *program, char *name, mc_vec2 value) {
	GLint loc = glGetUniformLocation(program->program, name);
	if (loc == -1) return;
	glUseProgram(program->program);
	glUniform2f(loc, value.x, value.y);
}

void mc_program_set_vec3(mc_Program *program, char *name, mc_vec3 value) {
	GLint loc = glGetUniformLocation(program->program, name);
	if (loc == -1) return;
	glUseProgram(program->program);
	glUniform3f(loc, value.x, value.y, value.z);
}

void mc_program_set_vec4(mc_Program *program, char *name, mc_vec4 value) {
	GLint loc = glGetUniformLocation(program->program, name);
	if (loc == -1) return;
	glUseProgram(program->program);
	glUniform4f(loc, value.x, value.y, value.z, value.w);
}

void mc_program_set_int(mc_Program *program, char *name, int value) {
	GLint loc = glGetUniformLocation(program->program, name);
	if (loc == -1) return;
	glUseProgram(program->program);
	glUniform1i(loc, value);
}

void mc_program_set_ivec2(mc_Program *program, char *name, mc_ivec2 value) {
	GLint loc = glGetUniformLocation(program->program, name);
	if (loc == -1) return;
	glUseProgram(program->program);
	glUniform2i(loc, value.x, value.y);
}

void mc_program_set_ivec3(mc_Program *program, char *name, mc_ivec3 value) {
	GLint loc = glGetUniformLocation(program->program, name);
	if (loc == -1) return;
	glUseProgram(program->program);
	glUniform3i(loc, value.x, value.y, value.z);
}

void mc_program_set_ivec4(mc_Program *program, char *name, mc_ivec4 value) {
	GLint loc = glGetUniformLocation(program->program, name);
	if (loc == -1) return;
	glUseProgram(program->program);
	glUniform4i(loc, value.x, value.y, value.z, value.w);
}