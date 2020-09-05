#include <windows.h>
#include <gl/gl.h>

extern void OpenGl_RenderFrame(void)
{
	glClearDepth(1.0f);
	glClearColor(0.2f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}