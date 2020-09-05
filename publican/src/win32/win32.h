#pragma once
#ifndef PUB_WIN32_H
#define PUB_WIN32_H

#include <windows.h>
#include <gl/gl.h>

#include "../platform.h"

/*
	memory.c

	The platform allocator must ensure that enough extra space
	is allocated after the platform block to store the extra
	fields for the win32 block
*/
struct win32_memory_block {
	struct platform_memory_block block;
	struct win32_memory_block *next;
	struct win32_memory_block *prev;
	uint64_t loopingFlags;
};

enum win32_mem_flags {
	WMEM_ALLOC_DURING_LOOP = 0x1,
	WMEM_FREE_DURING_LOOP = 0x2,
};

struct platform_memory_block *Win32_Alloc(size_t size, DWORDLONG flags);
void Win32_DeAlloc(struct platform_memory_block *block);

// thread.c

struct win32_work_queue {
	HANDLE semaphoreHandle;
};

void Win32_CompleteAllWork(struct platform_work_queue *queue);
void MakeQueue(struct platform_work_queue *queue, DWORD threadCount, struct win32_thread_startup *startups);
void Win32_AddEntry(struct platform_work_queue *queue, platform_work_queue_callback *callback, void *data);

// file.c

struct win32_file_handle {
	HANDLE win32Handle;
};

struct win32_file_group {
	HANDLE findHandle;
	WIN32_FIND_DATAW findData;
};

struct platform_file_group Win32_GetFilesTypeStart(enum platform_file_type type);
void Win32_GetFilesTypeEnd(struct platform_file_group *fileGroup);
void Win32_FileError(struct platform_file_handle *handle, char *message);
struct platform_file_handle Win32_OpenNextFile(struct platform_file_group *fileGroup);
void Win32_ReadDataFromFile(struct platform_file_handle *source, DWORDLONG offset, DWORDLONG size, void *dest);

struct win32_state {
	struct ticket_mutex memMutex;
	struct win32_memory_block memSentinal;
};

struct win32_state globalWin32State;

// wgl.c
HGLRC InitOpenGL(void);

#define WGL_DRAW_TO_WINDOW_ARB                  0x2001
#define WGL_ACCELERATION_ARB                    0x2003
#define WGL_SUPPORT_OPENGL_ARB                  0x2010
#define WGL_DOUBLE_BUFFER_ARB                   0x2011
#define WGL_PIXEL_TYPE_ARB                      0x2013

#define WGL_TYPE_RGBA_ARB                       0x202B
#define WGL_FULL_ACCELERATION_ARB               0x2027

#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB        0x20A9

#define WGL_RED_BITS_ARB                        0x2015
#define WGL_GREEN_BITS_ARB                      0x2017
#define WGL_BLUE_BITS_ARB                       0x2019
#define WGL_ALPHA_BITS_ARB                      0x201B
#define WGL_DEPTH_BITS_ARB                      0x2022

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

typedef BOOL WINAPI wgl_swap_interval_ext(int interval);

typedef const char * WINAPI wgl_get_extensions_string_ext(void);

typedef HGLRC WINAPI wgl_create_context_attribs_arb(
	HDC hDC,
	HGLRC hShareContext,
	const int *attribList);

typedef BOOL WINAPI wgl_get_pixel_format_attrib_iv_arb(
	HDC hdc,
	int iPixelFormat,
	int iLayerPlane,
	UINT nAttributes,
	const int *piAttributes,
	int *piValues);

typedef BOOL WINAPI wgl_get_pixel_format_attrib_fv_arb(
	HDC hdc,
	int iPixelFormat,
	int iLayerPlane,
	UINT nAttributes,
	const int *piAttributes,
	FLOAT *pfValues);

typedef BOOL WINAPI wgl_choose_pixel_format_arb(
	HDC hdc,
	const int *piAttribIList,
	const FLOAT *pfAttribFList,
	UINT nMaxFormats,
	int *piFormats,
	UINT *nNumFormats);

static wgl_create_context_attribs_arb *wglCreateContextAttribsARB;
static wgl_choose_pixel_format_arb *wglChoosePixelFormatARB;
static wgl_swap_interval_ext *wglSwapIntervalEXT;
static wgl_get_extensions_string_ext *wglGetExtensionsStringEXT;

static bool OpenGLSupportsSRGBFramebuffer = false;
static GLuint OpenGLDefaultInternalTextureFormat;
static GLuint OpenGLReservedBlitTexture;

#endif