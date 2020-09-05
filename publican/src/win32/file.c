/************************************************************************************
__________     ___.   .__  .__                      	Win32 File IO
\______   \__ _\_ |__ |  | |__| ____ _____    ____
 |     ___/  |  \ __ \|  | |  |/ ___\\__  \  /    \		*doesn't yet close file
 |    |   |  |  / \_\ \  |_|  \  \___ / __ \|   |  \	 handles, when to fix?
 |____|   |____/|___  /____/__|\___  >____  /___|  /
		    		\/             \/     \/     \/
*************************************************************************************/

#include <windows.h>

#include "../platform.h"
#include "../utils/utils.h"
#include "win32.h"

extern PLATFORM_GET_ALL_FILES_OF_TYPE_START(Win32_GetFilesTypeStart)
{
	struct platform_file_group result = {};

	struct win32_file_group *win32FileGroup = VirtualAlloc(0,
		sizeof(struct win32_file_group),
		MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	result.platform = win32FileGroup;

	WCHAR *wildCard = L"*.*";
	switch(type) {
	case FILETYPE_ASSET:
		wildCard = L"*.pubb";
		break;
	default: INVALID_PATH;
	}
	result.fileCount = 0;

	WIN32_FIND_DATAW findData;
	HANDLE findHandle = FindFirstFileW(wildCard, &findData);
	while(findHandle != INVALID_HANDLE_VALUE) {
		++result.fileCount;
		if(!FindNextFileW(findHandle, &findData)) {
			break;
		}
	}
	FindClose(findHandle);
	win32FileGroup->findHandle = FindFirstFileW(wildCard, &win32FileGroup->findData);

	return(result);
}

extern PLATFORM_GET_ALL_FILES_OF_TYPE_END(Win32_GetFilesTypeEnd)
{
	struct win32_file_group *win32FileGroup = (struct win32_file_group *)fileGroup->platform;

	if(win32FileGroup) {
		FindClose(win32FileGroup->findHandle);
		VirtualFree(win32FileGroup, 0, MEM_RELEASE);
		fileGroup->platform = NULL;
	}
}

extern PLATFORM_OPEN_FILE(Win32_OpenNextFile)
{
	struct win32_file_group *win32FileGroup = (struct win32_file_group *)fileGroup->platform;
	struct platform_file_handle result = {};

	if(win32FileGroup->findHandle != INVALID_HANDLE_VALUE) {
		struct win32_file_handle *win32Handle =	VirtualAlloc(0,
			sizeof(struct win32_file_handle),
			MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
		result.platform = win32Handle;

		if(win32Handle) {
			wchar_t *fileName = win32FileGroup->findData.cFileName;
			win32Handle->win32Handle = CreateFileW(fileName, GENERIC_READ,
				FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
			result.noErrors = (win32Handle->win32Handle != INVALID_HANDLE_VALUE);
		} if(!FindNextFileW(win32FileGroup->findHandle, &win32FileGroup->findData)) {
			FindClose(win32FileGroup->findHandle);
			win32FileGroup->findHandle = INVALID_HANDLE_VALUE;
		}
	}

	return(result);
}

extern PLATFORM_FILE_ERROR(Win32_FileError)
{
	handle->noErrors = false;
}

extern PLATFORM_READ_DATA_FROM_FILE(Win32_ReadDataFromFile)
{
	if(PlatformNoFileErrors(source)) {
		struct win32_file_handle *handle = (struct win32_file_handle *)source->platform;
		OVERLAPPED overlapped = {
			.Offset = (DWORD)((offset >> 0) & 0xFFFFFFFF),
			.OffsetHigh = (DWORD)((offset >> 32) & 0xFFFFFFFF)
		};

		DWORD fileSize32 = SafeTruncateUInt64(size);
		DWORD bytesRead;
		if(!ReadFile(handle->win32Handle, dest, fileSize32, &bytesRead, &overlapped) &&
		   (fileSize32 != bytesRead)) {
			Win32_FileError(source, "win32_ReadDataFromFile Failed\n");
		}
	}
}