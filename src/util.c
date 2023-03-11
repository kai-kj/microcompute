#include "_microcompute.h"

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