#pragma once
#ifndef PUB_PLATFORM_H
#define PUB_PLATFORM_H

#include <stdbool.h>
#include <stdint.h>

#define HIGH_PRIORITY_THREADS 6
#define LOW_PRIORITY_THREADS 2

/*
	Memory

	platform_memory_block:

*/

struct platform_memory_block {
	uint64_t flags;
	uint64_t size;
	uint8_t *base;
	uintptr_t used;
	struct platform_memory_block *arenaPrev;
};

#define PLATFORM_ALLOCATE_MEMORY(name) struct platform_memory_block *name(size_t size, uint64_t flags)
typedef PLATFORM_ALLOCATE_MEMORY(platform_allocate_memory);

#define PLATFORM_DEALLOCATE_MEMORY(name) void name(struct platform_memory_block *block)
typedef PLATFORM_DEALLOCATE_MEMORY(platform_deallocate_memory);

/*
	Threads
*/

struct platform_work_queue;
#define PLATFORM_WORK_QUEUE_CALLBACK(name) void name(struct platform_work_queue *queue, void *data)
typedef PLATFORM_WORK_QUEUE_CALLBACK(platform_work_queue_callback);

struct platform_work_queue_entry {
	platform_work_queue_callback *callback;
	void *data;
};

struct platform_work_queue {
	uint32_t volatile completionGoal;
	uint32_t volatile completionCount;

	uint32_t volatile nextToRead;;
	uint32_t volatile nextToWrite;

	struct platform_work_queue_entry entries[256];
	void *platform;
};

struct win32_thread_startup {
	struct platform_work_queue *queue;
};

struct ticket_mutex {
	uint64_t volatile ticket;
	uint64_t volatile serving;
};

typedef void platform_add_entry(struct platform_work_queue *queue, platform_work_queue_callback *callback, void *data);
typedef void platform_complete_all_work(struct platform_work_queue *queue);

/*
	Files
*/

enum platform_file_type {
	FILETYPE_ASSET,
	FILETYPE_COUNT,
};

struct platform_file_handle {
	bool noErrors;
	void *platform;
};

struct platform_file_group {
	uint32_t fileCount;
	void *platform;
};

struct file_read_output {
	uint32_t contentsSize;
	void *contents;
};

#define PLATFORM_GET_ALL_FILES_OF_TYPE_START(name) struct platform_file_group name(enum platform_file_type type)
typedef PLATFORM_GET_ALL_FILES_OF_TYPE_START(platform_get_all_files_of_type_start);

#define PLATFORM_GET_ALL_FILES_OF_TYPE_END(name) void name(struct platform_file_group *fileGroup)
typedef PLATFORM_GET_ALL_FILES_OF_TYPE_END(platform_get_all_files_of_type_end);

#define PLATFORM_OPEN_FILE(name) struct platform_file_handle name(struct platform_file_group *fileGroup)
typedef PLATFORM_OPEN_FILE(platform_open_next_file);

#define PLATFORM_READ_DATA_FROM_FILE(name) void name(struct platform_file_handle *source, uint64_t offset, uint64_t size, void *dest)
typedef PLATFORM_READ_DATA_FROM_FILE(platform_read_data_from_file);

#define PLATFORM_FILE_ERROR(name) void name(struct platform_file_handle *handle, char *message)
typedef PLATFORM_FILE_ERROR(platform_file_error);

#define PlatformNoFileErrors(handle) ((handle)->noErrors)

struct platform_api {
	platform_add_entry *AddTask;
	platform_complete_all_work *CompleteAllTasks;


	platform_get_all_files_of_type_start *GetAllFilesOfTypeStart;
	platform_get_all_files_of_type_end *GetAllFilesOfTypeEnd;
	platform_open_next_file *GetNextFile;
	platform_read_data_from_file *GetDataFromFile;

};

struct platform_api api;

#endif