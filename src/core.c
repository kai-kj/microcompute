#include "microcompute_internal.h"

struct mc_State {
    EGLDisplay disp;
    EGLContext ctx;
    mc_debug_cb* debug_cb;
    void* debug_cb_arg;
};

static struct mc_State S;

static void gl_debug_cb(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* arg
) {
    if (!S.debug_cb) return;

    mc_DebugLevel level;
    switch (severity) {
        case GL_DEBUG_SEVERITY_NOTIFICATION: level = MC_LVL_INFO; break;
        case GL_DEBUG_SEVERITY_LOW: level = MC_LVL_LOW; break;
        case GL_DEBUG_SEVERITY_MEDIUM: level = MC_LVL_MEDIUM; break;
        case GL_DEBUG_SEVERITY_HIGH: level = MC_LVL_HIGH; break;
        default: return;
    }

    uint32_t strLen = snprintf(NULL, 0, "GL: %s", message) + 1;
    char* str = malloc(strLen);
    snprintf(str, strLen, "GL: %s", message);

    S.debug_cb(level, str, S.debug_cb_arg);

    free(str);
}

char* mc_initialize(mc_debug_cb cb, void* arg) {
    S.debug_cb = cb;
    S.debug_cb_arg = arg;

    if (!gladLoaderLoadEGL(NULL)) return "failed to load egl";
    S.disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (S.disp == EGL_NO_DISPLAY) return "failed to get egl disp";

    EGLint major, minor;
    if (!eglInitialize(S.disp, &major, &minor)) return "failed to init egl";
    if (major < 1 || (major == 1 && minor < 5)) return "egl version too low";

    if (!gladLoaderLoadEGL(S.disp)) return "failed to reload egl";
    if (!eglBindAPI(EGL_OPENGL_API)) return "failed to bind opengl to egl";

    EGLConfig eglCfg;
    if (!eglChooseConfig(
            S.disp,
            (EGLint[]){EGL_NONE},
            &eglCfg,
            1,
            &(EGLint){0}
        ))
        return "failed to choose egl config";

    S.ctx = eglCreateContext(
        S.disp,
        eglCfg,
        EGL_NO_CONTEXT,
        (EGLint[]){
            EGL_CONTEXT_MAJOR_VERSION,
            4,
            EGL_CONTEXT_MINOR_VERSION,
            3,
            EGL_CONTEXT_OPENGL_DEBUG,
            EGL_TRUE,
            EGL_NONE,
        }
    );

    if (S.ctx == EGL_NO_CONTEXT) return "failed to create egl context";

    if (!eglMakeCurrent(S.disp, EGL_NO_SURFACE, EGL_NO_SURFACE, S.ctx))
        return "failed to make egl context current";

    if (gladLoaderLoadGL() == 0) return "failed to load gl";

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_cb, NULL);

    return NULL;
}

void mc_terminate() {
    if (S.ctx != 0) eglDestroyContext(S.disp, S.ctx);
    if (S.disp != 0) eglTerminate(S.disp);
}

double mc_wait() {
    double startTime = mc_get_time();
    glFinish();
    return mc_get_time() - startTime;
}

double mc_get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)(1000000 * tv.tv_sec + tv.tv_usec) / 1000000.0;
}