#include "_microcompute.h"

#ifdef STANDALONE
struct mc_State {
    int32_t rendererFd;
    struct gbm_device* device;
    EGLDisplay display;
    EGLContext context;
};

static struct mc_State state;

static void gl_debug_cb(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* msg,
    const void* data
) {
#ifdef DEBUG
    char* sourceStr = (char*[]){
        "API",
        "WINDOW",
        "SHADER",
        "THIRD PARTY",
        "APPLICATION",
        "UNKNOWN",
    }[source - GL_DEBUG_SOURCE_API];

    char* typeStr = (char*[]){
        "error",
        "deprecated",
        "undefined",
        "portability",
        "performance",
        "other",
        "marker",
    }[type - GL_DEBUG_TYPE_ERROR];

    printf("GL: %s (%s): %s\n", sourceStr, typeStr, msg);
#endif
}

mc_Result mc_start(char* renderDevice) {
    state.rendererFd = open(renderDevice, O_RDWR);
    ASSERT(state.rendererFd > 0, "failed to open render device");

    state.device = gbm_create_device(state.rendererFd);
    ASSERT(state.device != NULL, "failed to create gbm device");

    state.display = eglGetDisplay(state.device);
    ASSERT(state.display != NULL, "failed to get egl display");

    EGLint major, minor;
    ASSERT(
        eglInitialize(state.display, &major, &minor),
        "failed to initialize egl"
    );

    ASSERT(
        !(major < 1 || (major == 1 && minor < 5)),
        "egl version too low (min 1.5)"
    );

    ASSERT(eglBindAPI(EGL_OPENGL_API), "failed to bind opengl to egl");

    static const EGLint cfgAttrib[] = {
        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_BIT,
        EGL_NONE,
    };

    EGLConfig eglCfg;
    EGLint cfgCount;

    ASSERT(
        eglChooseConfig(state.display, cfgAttrib, &eglCfg, 1, &cfgCount),
        "failed to choose egl config"
    );

    static const EGLint ctxAttrib[] = {
        EGL_CONTEXT_MAJOR_VERSION,
        4,
        EGL_CONTEXT_MINOR_VERSION,
        6,
        EGL_CONTEXT_OPENGL_DEBUG,
        EGL_TRUE,
        EGL_NONE,
    };

    EGLContext ctx
        = eglCreateContext(state.display, eglCfg, EGL_NO_CONTEXT, ctxAttrib);

    ASSERT(ctx != EGL_NO_CONTEXT, "failed to create egl context");

    ASSERT(
        eglMakeCurrent(state.display, EGL_NO_SURFACE, EGL_NO_SURFACE, ctx),
        "failed to make egl context current"
    );

    gladLoadGL();

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_cb, NULL);

    return GL_CHECK_ERROR();
}

mc_Result mc_stop() {
    if (state.context != 0) eglDestroyContext(state.display, state.context);
    if (state.display != 0) eglTerminate(state.display);
    if (state.device != NULL) gbm_device_destroy(state.device);
    if (state.rendererFd != 0) close(state.rendererFd);
    return GL_CHECK_ERROR();
}

#endif

mc_Result mc_memory_barrier() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    return OK;
}