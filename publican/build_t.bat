@echo off
clang -std=c18 -Wall -O3 -Iinc -Llib -g -gcodeview ^
	src/win32/file.c ^
	src/win32/memory.c ^
	src/utils/string.c ^
	test/tests.c -o ^
test.exe -lcmocka
test.exe