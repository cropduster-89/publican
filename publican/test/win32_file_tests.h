#pragma once
#ifndef WIN32_FILE_TEST_H
#define WIN32_FILE_TEST_H

#include <windows.h>
#include <stdio.h>
#include "../src/win32/win32.h"

inline void Win32_GetFilesTypeStart_gets_all_files_of_type(void **state)
{
	char fileNames[][12] = {
		{"test1.pubb"},
		{"test2.pubb"},
		{"test3.pubb"},
		{"test4.cpp"},
	};
	for(int i = 0; i < 4; ++i) {
		FILE *fh;
		fopen_s(&fh, fileNames[i], "w");
		fclose(fh);
	}

	struct platform_file_group test = Win32_GetFilesTypeStart(FILETYPE_ASSET);

	assert_int_equal(test.fileCount, 3);

	for(int i = 0; i < 4; ++i) {
		remove(fileNames[i]);
	}
}

inline void Win32_GetFilesTypeEnd_frees_all_created_items(void **state)
{
	char fileNames[][12] = {
		{"test1.pubb"},
		{"test2.pubb"},
		{"test3.pubb"},
		{"test4.cpp"},
	};
	for(int i = 0; i < 4; ++i) {
		FILE *fh;
		fopen_s(&fh, fileNames[i], "w");
		fclose(fh);
	}

	struct platform_file_group data = Win32_GetFilesTypeStart(FILETYPE_ASSET);
	Win32_GetFilesTypeEnd(&data);

	assert_null(data.platform);

	for(int i = 0; i < 4; ++i) {
		remove(fileNames[i]);
	}
}

inline void Win32_OpenNextFile_can_open_a_file(void **state)
{
	char fileNames[][12] = {
		{"test1.pubb"},
		{"test2.pubb"},
		{"test3.pubb"},
		{"test4.cpp"},
	};
	for(int i = 0; i < 4; ++i) {
		FILE *fh;
		fopen_s(&fh, fileNames[i], "w");
		fclose(fh);
	}
	struct platform_file_group data = Win32_GetFilesTypeStart(FILETYPE_ASSET);

	struct platform_file_handle test = Win32_OpenNextFile(&data);

	assert_int_not_equal(0, ((struct win32_file_handle *)test.platform)->win32Handle);

	CloseHandle(((struct win32_file_handle *)test.platform)->win32Handle);
	Win32_GetFilesTypeEnd(&data);
	for(int i = 0; i < 4; ++i) {
		remove(fileNames[i]);
	}
}

inline void Win32_ReadDataFromFile_can_extract_correct_data_from_handle(void **state)
{
	FILE *fh;
	fopen_s(&fh, "testfile.pubb", "w");
	char *testBuffer = "test";
	fwrite(testBuffer, 4, 1, fh);
	fclose(fh);
	struct platform_file_group data = Win32_GetFilesTypeStart(FILETYPE_ASSET);
	struct platform_file_handle file = Win32_OpenNextFile(&data);

	char *test[4] = {};
	Win32_ReadDataFromFile(&file, 0, 4, test);

	assert_string_equal("test", test);

	CloseHandle(((struct win32_file_handle *)file.platform)->win32Handle);
	Win32_GetFilesTypeEnd(&data);
	remove("testfile.pubb");
}

#endif