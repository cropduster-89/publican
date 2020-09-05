#include <assert.h>
#include <windows.h>
#include <gl/gl.h>

#include "../platform.h"
#include "../opengl/opengl.h"
#include "win32.h"

#define WIN_X 900
#define WIN_Y 500

static BOOL boolRunning;


LRESULT CALLBACK WindowProc(HWND window,
							UINT message,
							WPARAM wParam,
							LPARAM lParam)
{
	LRESULT result = 0;

	switch(message) {
	case WM_DESTROY:
	case WM_CLOSE:
		boolRunning = FALSE;
		break;
	default:
		result = DefWindowProc(window, message, wParam, lParam);
	}

	return result;
}

static void ProcessMessage(void)
{
	MSG message;

	for(;;) {
		if(!PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
			break;
		}
		switch(message.message) {
		default:
			TranslateMessage(&message);
			DispatchMessage(&message);
			break;
		}
	}
}

static struct platform_api InitPlatformApi(void)
{
	struct platform_api newApi = {
		.AddTask = Win32_AddEntry,
		.CompleteAllTasks = Win32_CompleteAllWork,
		.GetAllFilesOfTypeStart = Win32_GetFilesTypeStart,
		.GetAllFilesOfTypeEnd = Win32_GetFilesTypeEnd
	};

	return newApi;
}

static HWND CreateMainWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wc = {
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_OWNDC,
		.lpfnWndProc = WindowProc,
		.hInstance = hInstance,
		.hCursor = LoadCursor(0, IDC_ARROW),
		.lpszClassName = "Pub"
	};
	assert(RegisterClassEx(&wc));

	HWND result = CreateWindowEx(0, "Pub", "Publican",
		WS_VISIBLE|WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT,	WIN_X, WIN_Y,
		NULL, NULL, hInstance, NULL);
	assert(result);

	return result;
}

static void RenderFrame(HDC dc)
{
	OpenGl_RenderFrame();
	SwapBuffers(dc);
}

int CALLBACK wWinMain(HINSTANCE hInstance,
					  HINSTANCE hPrevInstance,
					  LPWSTR lpCmdLine,
					  int nCmdShow)
{
	HWND window = CreateMainWindow(hInstance);
	InitOpenGL();

	api = InitPlatformApi();

	boolRunning = TRUE;
	while(boolRunning) {
		ProcessMessage();

		HDC dc = GetDC(window);
		RenderFrame(dc);
		ReleaseDC(window, dc);
	}
}