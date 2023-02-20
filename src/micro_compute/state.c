#include "_micro_compute.h"

struct mc_State {
	int rendererFd;
	struct gbm_device *gbmDevice;
	EGLDisplay eglDisplay;
	EGLContext eglContext;
	void (*_debug_callback)(int, char *);
};

static struct mc_State g_state;

void _gl_debug_cb(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *msg,
	const void *data
) {
	if (g_state._debug_callback == NULL) return;

	char *_source;
	switch (source) {
		case GL_DEBUG_SOURCE_API: _source = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: _source = "WINDOW SYSTEM"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: _source = "SHADER"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: _source = "THIRD PARTY"; break;
		case GL_DEBUG_SOURCE_APPLICATION: _source = "APPLICATION"; break;
		case GL_DEBUG_SOURCE_OTHER: _source = "UNKNOWN"; break;
		default: return;
	}

	char *_type;
	switch (type) {
		case GL_DEBUG_TYPE_ERROR: _type = "error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: _type = "deprecated"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: _type = "undefined"; break;
		case GL_DEBUG_TYPE_PORTABILITY: _type = "portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: _type = "performance"; break;
		case GL_DEBUG_TYPE_OTHER: _type = "other"; break;
		case GL_DEBUG_TYPE_MARKER: _type = "marker"; break;
		default: return;
	}

	int _level;
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: _level = 4; break;
		case GL_DEBUG_SEVERITY_MEDIUM: _level = 3; break;
		case GL_DEBUG_SEVERITY_LOW: _level = 2; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: _level = 1; break;
		default: return;
	}

	int len = snprintf(
		NULL,
		0,
		"GL: %s (%s, level %d): %s",
		_source,
		_type,
		_level,
		msg
	);

	char *buff = malloc(len + 1);

	snprintf(
		buff,
		len + 1,
		"GL: %s (%s, level %d): %s",
		_source,
		_type,
		_level,
		msg
	);

	g_state._debug_callback(_level, buff);
	free(buff);
}

int mc_init(char *renderDevice) {
	g_state.rendererFd = open("/dev/dri/renderD129", O_RDWR);
	if (g_state.rendererFd <= 0) {
		mc_terminate();
		return 0;
	}

	g_state.gbmDevice = gbm_create_device(g_state.rendererFd);
	if (g_state.gbmDevice == NULL) {
		mc_terminate();
		return 0;
	}

	g_state.eglDisplay = eglGetDisplay(g_state.gbmDevice);
	if (g_state.eglDisplay == NULL) {
		mc_terminate();
		return 0;
	}

	EGLint major, minor;
	if (!eglInitialize(g_state.eglDisplay, &major, &minor)) {
		mc_terminate();
		return 0;
	};

	if (major < 1 || (major == 1 && minor < 5)) {
		mc_terminate();
		return 0;
	}

	if (!eglBindAPI(EGL_OPENGL_API)) {
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
		mc_terminate();
		return 0;
	};

	if (!eglMakeCurrent(
			g_state.eglDisplay,
			EGL_NO_SURFACE,
			EGL_NO_SURFACE,
			eglContext
		)) {
		mc_terminate();
		return 0;
	};

	glewInit();

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(_gl_debug_cb, NULL);

	return 1;
}

void mc_terminate() {
	if (g_state.eglDisplay != 0 && g_state.eglContext != 0)
		eglDestroyContext(g_state.eglDisplay, g_state.eglContext);
	if (g_state.eglDisplay != 0) eglTerminate(g_state.eglDisplay);
	if (g_state.gbmDevice != NULL) gbm_device_destroy(g_state.gbmDevice);
	if (g_state.rendererFd != 0) close(g_state.rendererFd);
}

void mc_set_debug_callback(void (*callback)(int, char *)) {
	g_state._debug_callback = callback;
}