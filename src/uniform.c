#include "_microcompute.h"

#define SET(program, name, fn, ...)                                            \
	do {                                                                       \
		GLint loc = glGetUniformLocation(program->program, name);              \
		ASSERT(loc != -1, "failed to find uniform");                           \
		glUseProgram(program->program);                                        \
		fn(loc, __VA_ARGS__);                                                  \
		return GL_CHECK_ERROR();                                               \
	} while (0)

#define SET_MATRIX(program, name, fn, v)                                       \
	SET(program, name, fn, 1, v.transpose, v.values)

mc_Result mc_program_set_float(mc_Program* program, char* name, float v) {
	SET(program, name, glUniform1f, v);
}

mc_Result mc_program_set_vec2(mc_Program* program, char* name, mc_vec2 v) {
	SET(program, name, glUniform2f, v.x, v.y);
}

mc_Result mc_program_set_vec3(mc_Program* program, char* name, mc_vec3 v) {
	SET(program, name, glUniform3f, v.x, v.y, v.z);
}

mc_Result mc_program_set_vec4(mc_Program* program, char* name, mc_vec4 v) {
	SET(program, name, glUniform4f, v.x, v.y, v.z, v.w);
}

mc_Result mc_program_set_int(mc_Program* program, char* name, int32_t v) {
	SET(program, name, glUniform1i, v);
}

mc_Result mc_program_set_ivec2(mc_Program* program, char* name, mc_ivec2 v) {
	SET(program, name, glUniform2i, v.x, v.y);
}

mc_Result mc_program_set_ivec3(mc_Program* program, char* name, mc_ivec3 v) {
	SET(program, name, glUniform3i, v.x, v.y, v.z);
}

mc_Result mc_program_set_ivec4(mc_Program* program, char* name, mc_ivec4 v) {
	SET(program, name, glUniform4i, v.x, v.y, v.z, v.w);
}

mc_Result mc_program_set_uint(mc_Program* program, char* name, uint32_t v) {
	SET(program, name, glUniform1ui, v);
}

mc_Result mc_program_set_uvec2(mc_Program* program, char* name, mc_uvec2 v) {
	SET(program, name, glUniform2ui, v.x, v.y);
}

mc_Result mc_program_set_uvec3(mc_Program* program, char* name, mc_uvec3 v) {
	SET(program, name, glUniform3ui, v.x, v.y, v.z);
}

mc_Result mc_program_set_uvec4(mc_Program* program, char* name, mc_uvec4 v) {
	SET(program, name, glUniform4ui, v.x, v.y, v.z, v.w);
}

mc_Result mc_program_set_mat22(mc_Program* program, char* name, mc_mat22 v) {
	SET_MATRIX(program, name, glUniformMatrix2fv, v);
}

mc_Result mc_program_set_mat33(mc_Program* program, char* name, mc_mat33 v) {
	SET_MATRIX(program, name, glUniformMatrix3fv, v);
}

mc_Result mc_program_set_mat44(mc_Program* program, char* name, mc_mat44 v) {
	SET_MATRIX(program, name, glUniformMatrix4fv, v);
}

mc_Result mc_program_set_mat23(mc_Program* program, char* name, mc_mat23 v) {
	SET_MATRIX(program, name, glUniformMatrix2x3fv, v);
}

mc_Result mc_program_set_mat32(mc_Program* program, char* name, mc_mat32 v) {
	SET_MATRIX(program, name, glUniformMatrix3x2fv, v);
}

mc_Result mc_program_set_mat24(mc_Program* program, char* name, mc_mat24 v) {
	SET_MATRIX(program, name, glUniformMatrix2x4fv, v);
}

mc_Result mc_program_set_mat42(mc_Program* program, char* name, mc_mat42 v) {
	SET_MATRIX(program, name, glUniformMatrix4x2fv, v);
}

mc_Result mc_program_set_mat34(mc_Program* program, char* name, mc_mat34 v) {
	SET_MATRIX(program, name, glUniformMatrix3x4fv, v);
}

mc_Result mc_program_set_mat43(mc_Program* program, char* name, mc_mat43 v) {
	SET_MATRIX(program, name, glUniformMatrix4x3fv, v);
}
