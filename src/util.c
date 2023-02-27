#include "_microcompute.h"

mc_Result mc_read_file(uint32_t* size, char* contents, const char* path) {
	FILE* fp = fopen(path, "rb");
	if (fp == NULL) return ERROR("failed to open file");

	fseek(fp, 0, SEEK_END);
	long length = ftell(fp) + 1;
	fseek(fp, 0, SEEK_SET);

	if (contents != NULL) {
		fread(contents, 1, length, fp);
		contents[length - 1] = '\0';
	}
	fclose(fp);

	if (size != NULL) *size = length;

	return OK;
}

void mc_result_pretty_print(mc_Result result) {
	if (result.ok) {
		printf(
			"\e[1m\e[35m%s\e[39m, \e[4m%s:%d\e[24m\e[39m, \e[32mok\e[39m: "
			"\e[0m%s\n",
			result.func,
			result.file,
			result.line,
			result.message
		);
	} else {
		printf(
			"\e[1m\e[35m%s\e[39m, \e[4m%s:%d\e[24m\e[39m, \e[31mERROR\e[39m: "
			"\e[0m%s\n",
			result.func,
			result.file,
			result.line,
			result.message
		);
	}
}

mc_Result gl_check_error(uint32_t line, const char* file, const char* func) {
	GLenum error = glGetError();
	if (error == GL_NO_ERROR)
		return ((mc_Result){
			.ok = MC_TRUE,
			.file = file,
			.line = line,
			.func = func,
			.message = (char*){"no errors here :)"}});

	else
		return ((mc_Result){
			.ok = MC_FALSE,
			.file = file,
			.line = line,
			.func = func,
			.message = (char*[]){
				"gl error: GL_INVALID_ENUM",
				"gl error: GL_INVALID_VALUE",
				"gl error: GL_INVALID_OPERATION",
				"gl error: GL_STACK_OVERFLOW",
				"gl error: GL_STACK_UNDERFLOW",
				"gl error: GL_OUT_OF_MEMORY",
				"gl error: GL_INVALID_FRAMEBUFFER_OPERATION",
				"gl error: GL_CONTEXT_LOST",
			}[error - GL_INVALID_ENUM]});
}