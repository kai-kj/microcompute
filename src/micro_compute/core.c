#include "_micro_compute.h"

struct mc_State {
	int rendererFd;
	struct gbm_device *gbmDevice;
	EGLDisplay eglDisplay;
	EGLContext eglContext;
	void (*debug_callback)(mc_DebugLevel, char *);
};

static struct mc_State g_state;

static void gl_debug_cb(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *msg,
	const void *data
) {
	if (g_state.debug_callback == NULL) return;

	char *sourceStr;
	switch (source) {
		case GL_DEBUG_SOURCE_API: sourceStr = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceStr = "WINDOW"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceStr = "SHADER"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: sourceStr = "THIRD PARTY"; break;
		case GL_DEBUG_SOURCE_APPLICATION: sourceStr = "APPLICATION"; break;
		case GL_DEBUG_SOURCE_OTHER: sourceStr = "UNKNOWN"; break;
		default: return;
	}

	char *typeStr;
	switch (type) {
		case GL_DEBUG_TYPE_ERROR: typeStr = "error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeStr = "deprecated"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeStr = "undefined"; break;
		case GL_DEBUG_TYPE_PORTABILITY: typeStr = "portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: typeStr = "performance"; break;
		case GL_DEBUG_TYPE_OTHER: typeStr = "other"; break;
		case GL_DEBUG_TYPE_MARKER: typeStr = "marker"; break;
		default: return;
	}

	mc_DebugLevel level;
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: level = mc_DebugLevel_HIGH; break;
		case GL_DEBUG_SEVERITY_MEDIUM: level = mc_DebugLevel_MEDIUM; break;
		case GL_DEBUG_SEVERITY_LOW: level = mc_DebugLevel_LOW; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: level = mc_DebugLevel_INFO; break;
		default: return;
	}

	int len = snprintf(
		NULL,
		0,
		"GL: %s (%s, level %d): %s",
		sourceStr,
		typeStr,
		level,
		msg
	);

	char *buff = malloc(len + 1);

	snprintf(
		buff,
		len + 1,
		"GL: %s (%s, level %d): %s",
		sourceStr,
		typeStr,
		level,
		msg
	);

	g_state.debug_callback(level, buff);
	free(buff);
}

void debug_msg_fn(const char *function, int level, char *format, ...) {
	if (g_state.debug_callback == NULL) return;

	va_list args;
	va_start(args, format);
	int len = vsnprintf(NULL, 0, format, args) + 1;
	va_end(args);

	char *message = malloc(len);

	va_start(args, format);
	vsnprintf(message, len, format, args);
	va_end(args);

	len = snprintf(NULL, 0, "%s(): %s", function, message);
	char *formattedMessage = malloc(len + 1);
	snprintf(formattedMessage, len + 1, "%s(): %s", function, message);

	g_state.debug_callback(level, formattedMessage);
	free(message);
	free(formattedMessage);
}

int mc_init(char *renderDevice) {
	g_state.rendererFd = open(renderDevice, O_RDWR);
	if (g_state.rendererFd <= 0) {
		debug_msg(mc_DebugLevel_HIGH, "failed to open %s", renderDevice);
		mc_terminate();
		return 0;
	}

	g_state.gbmDevice = gbm_create_device(g_state.rendererFd);
	if (g_state.gbmDevice == NULL) {
		debug_msg(mc_DebugLevel_HIGH, "failed to create gbm device");
		mc_terminate();
		return 0;
	}

	g_state.eglDisplay = eglGetDisplay(g_state.gbmDevice);
	if (g_state.eglDisplay == NULL) {
		debug_msg(mc_DebugLevel_HIGH, "failed to get egl display");
		mc_terminate();
		return 0;
	}

	EGLint major, minor;
	if (!eglInitialize(g_state.eglDisplay, &major, &minor)) {
		debug_msg(mc_DebugLevel_HIGH, "failed to initialize egl");
		mc_terminate();
		return 0;
	};

	if (major < 1 || (major == 1 && minor < 5)) {
		debug_msg(
			mc_DebugLevel_HIGH,
			"egl version: %d.%d too low (min 1.5)",
			major,
			minor
		);
		mc_terminate();
		return 0;
	}

	if (!eglBindAPI(EGL_OPENGL_API)) {
		debug_msg(mc_DebugLevel_HIGH, "failed to bind opengl api to egl");
		mc_terminate();
		return 0;
	}

	static const EGLint configAttributes[] = {
		EGL_RENDERABLE_TYPE,
		EGL_OPENGL_BIT,
		EGL_NONE,
	};

	EGLConfig eglConfig;
	EGLint configCount;

	if (!eglChooseConfig(
			g_state.eglDisplay,
			configAttributes,
			&eglConfig,
			1,
			&configCount
		)) {
		debug_msg(mc_DebugLevel_HIGH, "failed to choose egl config");
		mc_terminate();
		return 0;
	};

	static const EGLint contextAttributes[] = {
		EGL_CONTEXT_MAJOR_VERSION,
		4,
		EGL_CONTEXT_MINOR_VERSION,
		5,
		EGL_CONTEXT_OPENGL_DEBUG,
		EGL_TRUE,
		EGL_NONE,
	};

	EGLContext eglContext = eglCreateContext(
		g_state.eglDisplay,
		eglConfig,
		EGL_NO_CONTEXT,
		contextAttributes
	);

	if (eglContext == EGL_NO_CONTEXT) {
		debug_msg(mc_DebugLevel_HIGH, "failed to create egl context");
		mc_terminate();
		return 0;
	};

	if (!eglMakeCurrent(
			g_state.eglDisplay,
			EGL_NO_SURFACE,
			EGL_NO_SURFACE,
			eglContext
		)) {
		debug_msg(mc_DebugLevel_HIGH, "failed to make egl context current");
		mc_terminate();
		return 0;
	};

	if (!glewInit()) {
		debug_msg(mc_DebugLevel_HIGH, "failed to initialize GLEW");
		mc_terminate();
		return 0;
	}

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(gl_debug_cb, NULL);

	return 1;
}

void mc_terminate() {
	if (g_state.eglDisplay != 0 && g_state.eglContext != 0)
		eglDestroyContext(g_state.eglDisplay, g_state.eglContext);
	if (g_state.eglDisplay != 0) eglTerminate(g_state.eglDisplay);
	if (g_state.gbmDevice != NULL) gbm_device_destroy(g_state.gbmDevice);
	if (g_state.rendererFd != 0) close(g_state.rendererFd);
}

void mc_set_debug_callback(void (*callback)(mc_DebugLevel, char *)) {
	g_state.debug_callback = callback;
}