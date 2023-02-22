#include "_microcompute.h"

struct mc_State {
	int rendererFd;
	struct gbm_device *device;
	EGLDisplay display;
	EGLContext context;
	void (*debug_cb)(mc_DebugLevel, char *, void *);
	void *callbackArg;
};

static struct mc_State state;

static void gl_debug_cb(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *msg,
	const void *data
) {
	if (state.debug_cb == NULL) return;

	char *sourceStr = (char *[]){
		"API",
		"WINDOW",
		"SHADER",
		"THIRD PARTY",
		"APPLICATION",
		"UNKNOWN",
	}[source - GL_DEBUG_SOURCE_API];

	char *typeStr = (char *[]){
		"error",
		"deprecated",
		"undefined",
		"portability",
		"performance",
		"other",
		"marker",
	}[type - GL_DEBUG_TYPE_ERROR];

	mc_DebugLevel level;
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: level = mc_DebugLevel_HIGH; break;
		case GL_DEBUG_SEVERITY_MEDIUM: level = mc_DebugLevel_MEDIUM; break;
		case GL_DEBUG_SEVERITY_LOW: level = mc_DebugLevel_LOW; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: level = mc_DebugLevel_INFO; break;
		default: return;
	}

	int len = snprintf(NULL, 0, "GL: %s (%s): %s", sourceStr, typeStr, msg) + 1;
	char *buff = malloc(len);
	snprintf(buff, len, "GL: %s (%s): %s", sourceStr, typeStr, msg);

	state.debug_cb(level, buff, state.callbackArg);
	free(buff);
}

void debug_msg_fn(const char *function, int level, char *format, ...) {
	if (state.debug_cb == NULL) return;

	va_list args;
	va_start(args, format);
	int len = vsnprintf(NULL, 0, format, args) + 1;
	va_end(args);
	char *message = malloc(len);
	va_start(args, format);
	vsnprintf(message, len, format, args);
	va_end(args);

	len = snprintf(NULL, 0, "%s(): %s", function, message) + 1;
	char *formattedMessage = malloc(len);
	snprintf(formattedMessage, len, "%s(): %s", function, message);

	state.debug_cb(level, formattedMessage, state.callbackArg);
	free(message);
	free(formattedMessage);
}

bool mc_start(char *renderDevice) {
	state.rendererFd = open(renderDevice, O_RDWR);
	if (state.rendererFd <= 0) {
		debug_msg(mc_DebugLevel_HIGH, "failed to open %s", renderDevice);
		mc_stop();
		return false;
	}

	state.device = gbm_create_device(state.rendererFd);
	if (state.device == NULL) {
		debug_msg(mc_DebugLevel_HIGH, "failed to create gbm device");
		mc_stop();
		return false;
	}

	state.display = eglGetDisplay(state.device);
	if (state.display == NULL) {
		debug_msg(mc_DebugLevel_HIGH, "failed to get egl display");
		mc_stop();
		return false;
	}

	EGLint major, minor;
	if (!eglInitialize(state.display, &major, &minor)) {
		debug_msg(mc_DebugLevel_HIGH, "failed to initialize egl");
		mc_stop();
		return false;
	};

	if (major < 1 || (major == 1 && minor < 5)) {
		debug_msg(
			mc_DebugLevel_HIGH,
			"egl version: %d.%d too low (min req: 1.5)",
			major,
			minor
		);
		mc_stop();
		return false;
	}

	if (!eglBindAPI(EGL_OPENGL_API)) {
		debug_msg(mc_DebugLevel_HIGH, "failed to bind opengl api to egl");
		mc_stop();
		return false;
	}

	static const EGLint cfgAttrib[] = {
		EGL_RENDERABLE_TYPE,
		EGL_OPENGL_BIT,
		EGL_NONE,
	};

	EGLConfig eglCfg;
	EGLint cfgCount;

	if (!eglChooseConfig(state.display, cfgAttrib, &eglCfg, 1, &cfgCount)) {
		debug_msg(mc_DebugLevel_HIGH, "failed to choose egl config");
		mc_stop();
		return false;
	};

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

	if (ctx == EGL_NO_CONTEXT) {
		debug_msg(mc_DebugLevel_HIGH, "failed to create egl context");
		mc_stop();
		return false;
	};

	if (!eglMakeCurrent(state.display, EGL_NO_SURFACE, EGL_NO_SURFACE, ctx)) {
		debug_msg(mc_DebugLevel_HIGH, "failed to make egl context current");
		mc_stop();
		return false;
	};

	if (!glewInit()) {
		debug_msg(mc_DebugLevel_HIGH, "failed to initialize GLEW");
		mc_stop();
		return false;
	}

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(gl_debug_cb, NULL);

	return true;
}

void mc_stop() {
	if (state.context != 0) eglDestroyContext(state.display, state.context);
	if (state.display != 0) eglTerminate(state.display);
	if (state.device != NULL) gbm_device_destroy(state.device);
	if (state.rendererFd != 0) close(state.rendererFd);
}

void mc_set_debug_callback(mc_DebugCallback cb, void *arg) {
	state.debug_cb = cb;
	state.callbackArg = arg;
}

void mc_default_debug_callback(mc_DebugLevel level, char *msg, void *arg) {
	if (arg == NULL || level >= *(mc_DebugLevel *)arg)
		printf("DEBUG (level %d): %s\n", level, msg);
}
