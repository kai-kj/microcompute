#include "_microcompute.h"

char* mc_read_file(const char* path, uint32_t* size) {
    FILE* fp = fopen(path, "rb");
    if (fp == NULL) return NULL;

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp) + 1;
    fseek(fp, 0, SEEK_SET);

    char* contents = malloc(length);
    length = fread(contents, 1, length, fp);
    contents[length - 1] = '\0';
    fclose(fp);

    if (size != NULL) *size = length;
    return contents;
}

k_Result gl_check_error(uint32_t line, const char* file, const char* func) {
    GLenum error = glGetError();
    if (error == GL_NO_ERROR)
        return ((k_Result){
            .ok = K_TRUE,
            .file = file,
            .line = line,
            .message = (char*){"no errors here :)"}});

    else
        return ((k_Result){
            .ok = K_FALSE,
            .file = file,
            .line = line,
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