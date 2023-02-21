#include "_microcompute.h"

#define SET(program, name, uniformFn, ...)                                     \
	GLint loc = glGetUniformLocation(program->program, name);                  \
	if (loc == -1) {                                                           \
		debug_msg(mc_DebugLevel_MEDIUM, "failed to find uniform %s", name);    \
		return;                                                                \
	}                                                                          \
	glUseProgram(program->program);                                            \
	uniformFn(loc, __VA_ARGS__)

void mc_program_set_float(mc_Program *program, char *name, float value) {
	SET(program, name, glUniform1f, value);
}

void mc_program_set_vec2(mc_Program *program, char *name, mc_vec2 value) {
	SET(program, name, glUniform2f, value.x, value.y);
}

void mc_program_set_vec3(mc_Program *program, char *name, mc_vec3 value) {
	SET(program, name, glUniform3f, value.x, value.y, value.z);
}

void mc_program_set_vec4(mc_Program *program, char *name, mc_vec4 value) {
	SET(program, name, glUniform4f, value.x, value.y, value.z, value.w);
}

void mc_program_set_int(mc_Program *program, char *name, int value) {
	SET(program, name, glUniform1i, value);
}

void mc_program_set_ivec2(mc_Program *program, char *name, mc_ivec2 value) {
	SET(program, name, glUniform2i, value.x, value.y);
}

void mc_program_set_ivec3(mc_Program *program, char *name, mc_ivec3 value) {
	SET(program, name, glUniform3i, value.x, value.y, value.z);
}

void mc_program_set_ivec4(mc_Program *program, char *name, mc_ivec4 value) {
	SET(program, name, glUniform4i, value.x, value.y, value.z, value.w);
}