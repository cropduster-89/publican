@echo off
clang -Wall -std=c18 -O0 -g -gcodeview ^
src/opengl/render.c ^
src/utils/string.c ^
src/win32/main.c ^
src/win32/wgl.c ^
src/win32/thread.c ^
src/win32/memory.c ^
src/win32/file.c -o pub.exe -luser32 -lgdi32 -lopengl32