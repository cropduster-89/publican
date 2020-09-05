/************************************************************************************
__________     ___.   .__  .__                            OpenGL Setup for windows
\______   \__ _\_ |__ |  | |__| ____ _____    ____
 |     ___/  |  \ __ \|  | |  |/ ___\\__  \  /    \
 |    |   |  |  / \_\ \  |_|  \  \___ / __ \|   |  \
 |____|   |____/|___  /____/__|\___  >____  /___|  /
		   			\/             \/     \/     \/
*************************************************************************************/

#include <windows.h>
#include <gl/gl.h>
#include <assert.h>

#include "win32.h"
#include "../utils/utils.h"

#define GL_ATTRIBS (int32_t[]) { \
	WGL_CONTEXT_MAJOR_VERSION_ARB, 3, \
	WGL_CONTEXT_MINOR_VERSION_ARB, 3, \
	WGL_CONTEXT_FLAGS_ARB, 0, \
	WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB, \
	0, \
}

#define WGL_ATTRIBS (int32_t[]) { \
	WGL_DRAW_TO_WINDOW_ARB, GL_TRUE, \
	WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, \
	WGL_SUPPORT_OPENGL_ARB, GL_TRUE, \
	WGL_DOUBLE_BUFFER_ARB, GL_TRUE, \
	WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, \
	WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE, \
	0, \
}

/*
*	Makes sure windows supports the desired pixel format for
*	the opengl context, then sets that to the HDC
*	TODO: What to do when unsupported? Research
*/
static void InitPixelFormat(const HDC dc)
{
	int32_t suggestedFormatIndex = 0;
	GLuint extendedPick = 0;

	if(wglChoosePixelFormatARB) {
		int32_t *pixFormatAttribs = WGL_ATTRIBS;
		if(!OpenGLSupportsSRGBFramebuffer) {
			pixFormatAttribs[10] = 0;
		}
		wglChoosePixelFormatARB(dc, pixFormatAttribs, 0, 1,
			&suggestedFormatIndex, &extendedPick);
	}

	if(!extendedPick) {
		PIXELFORMATDESCRIPTOR desiredFormat = {
			.nSize = sizeof(desiredFormat),
			.nVersion = 1,
			.iPixelType = PFD_TYPE_RGBA,
			.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER,
			.cColorBits = 32,
			.cAlphaBits = 8,
			.iLayerType = PFD_MAIN_PLANE
		};
		suggestedFormatIndex = ChoosePixelFormat(dc, &desiredFormat);
	}

	PIXELFORMATDESCRIPTOR suggestedFormat;
	DescribePixelFormat(dc, suggestedFormatIndex, sizeof(suggestedFormat), &suggestedFormat);
	SetPixelFormat(dc, suggestedFormatIndex, &suggestedFormat);
}

static HWND GetDummyWindow(void)
{
	WNDCLASSA dummy = {};
	dummy.lpfnWndProc = DefWindowProcA;
	dummy.hInstance = GetModuleHandle(0);
	dummy.lpszClassName = "dummyWGL";
	assert(RegisterClassA(&dummy));

	return CreateWindowA(dummy.lpszClassName,
		"", 0, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
		dummy.hInstance, 0);
}

static void GetFramebufferAvailability(char *const extensions)
{
	char *at = extensions;
	while(*at) {
		while(IsWhitespace(*at)) {++at;}
		char *end = at;
		while(*end && !IsWhitespace(*end)) {++end;}
		uintptr_t count = end - at;

		if(StringsAreEqual(count, at, "WGL_EXT_framebuffer_sRGB")) {
			OpenGLSupportsSRGBFramebuffer = true;

		} else if(StringsAreEqual(count, at, "WGL_ARB_framebuffer_sRGB")) {
			OpenGLSupportsSRGBFramebuffer = true;
		}
		at = end;
	}
}

static void LoadWglFunctions(void)
{
	wglChoosePixelFormatARB = (wgl_choose_pixel_format_arb *)wglGetProcAddress("wglChoosePixelFormatARB");
	wglCreateContextAttribsARB = (wgl_create_context_attribs_arb *)wglGetProcAddress("wglCreateContextAttribsARB");
	wglSwapIntervalEXT = (wgl_swap_interval_ext *)wglGetProcAddress("wglSwapIntervalEXT");
	wglGetExtensionsStringEXT = (wgl_get_extensions_string_ext *)wglGetProcAddress("wglGetExtensionsStringEXT");
}

/*
*	Creates dummy window & context used for loading the wgl extensions,
*	which are in turn used to load the opengl extensions
*/
static void LoadWGLExtensions(void)
{
	HWND dummyWindow = GetDummyWindow();
	HDC dc = GetDC(dummyWindow);
	InitPixelFormat(dc);
	HGLRC openGLRC = wglCreateContext(dc);

	if(wglMakeCurrent(dc, openGLRC)) {
		LoadWglFunctions();
		if(wglGetExtensionsStringEXT) {
			GetFramebufferAvailability((char *)wglGetExtensionsStringEXT());
		}
		wglMakeCurrent(0, 0);
	} else {
		INVALID_PATH;
	}

	wglDeleteContext(openGLRC);
	ReleaseDC(dummyWindow, dc);
	DestroyWindow(dummyWindow);
}

extern HGLRC InitOpenGL(void)
{
	HWND hwnd = GetActiveWindow();
	HDC dc = GetDC(hwnd);

	LoadWGLExtensions();
	InitPixelFormat(dc);

	bool modernContext = true;
	HGLRC openGLRC = 0;
	if(wglCreateContextAttribsARB) {
		openGLRC = wglCreateContextAttribsARB(dc, 0, GL_ATTRIBS);
	} if(!openGLRC) {
		modernContext = false;
		openGLRC = wglCreateContext(dc);
	} if(wglMakeCurrent(dc, openGLRC)) {

	} else {
		INVALID_PATH;
	}
	ReleaseDC(hwnd, dc);

	return(openGLRC);
}