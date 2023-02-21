#include "_microcompute.h"

#define SET(program, name, fn, ...)                                            \
	GLint loc = glGetUniformLocation(program->program, name);                  \
	if (loc == -1) {                                                           \
		debug_msg(mc_DebugLevel_MEDIUM, "failed to find uniform %s", name);    \
		return false;                                                          \
	}                                                                          \
	glUseProgram(program->program);                                            \
	fn(loc, __VA_ARGS__);                                                      \
	return true;

#define SET_MATRIX(program, name, fn, value)                                   \
	SET(program, name, fn, 1, value.transpose, value.values)

bool mc_program_set_float(mc_Program *program, char *name, float value) {
	SET(program, name, glUniform1f, value);
}

bool mc_program_set_vec2(mc_Program *program, char *name, mc_vec2 value) {
	SET(program, name, glUniform2f, value.x, value.y);
}

bool mc_program_set_vec3(mc_Program *program, char *name, mc_vec3 value) {
	SET(program, name, glUniform3f, value.x, value.y, value.z);
}

bool mc_program_set_vec4(mc_Program *program, char *name, mc_vec4 value) {
	SET(program, name, glUniform4f, value.x, value.y, value.z, value.w);
}

bool mc_program_set_int(mc_Program *program, char *name, int value) {
	SET(program, name, glUniform1i, value);
}

bool mc_program_set_ivec2(mc_Program *program, char *name, mc_ivec2 value) {
	SET(program, name, glUniform2i, value.x, value.y);
}

bool mc_program_set_ivec3(mc_Program *program, char *name, mc_ivec3 value) {
	SET(program, name, glUniform3i, value.x, value.y, value.z);
}

bool mc_program_set_ivec4(mc_Program *program, char *name, mc_ivec4 value) {
	SET(program, name, glUniform4i, value.x, value.y, value.z, value.w);
}

bool mc_program_set_uint(mc_Program *program, char *name, unsigned int value) {
	SET(program, name, glUniform1ui, value);
}

bool mc_program_set_uvec2(mc_Program *program, char *name, mc_uvec2 value) {
	SET(program, name, glUniform2ui, value.x, value.y);
}

bool mc_program_set_uvec3(mc_Program *program, char *name, mc_uvec3 value) {
	SET(program, name, glUniform3ui, value.x, value.y, value.z);
}

bool mc_program_set_uvec4(mc_Program *program, char *name, mc_uvec4 value) {
	SET(program, name, glUniform4ui, value.x, value.y, value.z, value.w);
}

bool mc_program_set_mat22(mc_Program *program, char *name, mc_mat22 value) {
	SET_MATRIX(program, name, glUniformMatrix2fv, value);
}

bool mc_program_set_mat33(mc_Program *program, char *name, mc_mat33 value) {
	SET_MATRIX(program, name, glUniformMatrix3fv, value);
}

bool mc_program_set_mat44(mc_Program *program, char *name, mc_mat44 value) {
	SET_MATRIX(program, name, glUniformMatrix4fv, value);
}

bool mc_program_set_mat23(mc_Program *program, char *name, mc_mat23 value) {
	SET_MATRIX(program, name, glUniformMatrix2x3fv, value);
}

bool mc_program_set_mat32(mc_Program *program, char *name, mc_mat32 value) {
	SET_MATRIX(program, name, glUniformMatrix3x2fv, value);
}

bool mc_program_set_mat24(mc_Program *program, char *name, mc_mat24 value) {
	SET_MATRIX(program, name, glUniformMatrix2x4fv, value);
}

bool mc_program_set_mat42(mc_Program *program, char *name, mc_mat42 value) {
	SET_MATRIX(program, name, glUniformMatrix4x2fv, value);
}

bool mc_program_set_mat34(mc_Program *program, char *name, mc_mat34 value) {
	SET_MATRIX(program, name, glUniformMatrix3x4fv, value);
}

bool mc_program_set_mat43(mc_Program *program, char *name, mc_mat43 value) {
	SET_MATRIX(program, name, glUniformMatrix4x3fv, value);
}
