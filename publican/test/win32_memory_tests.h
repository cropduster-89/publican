#pragma once
#ifndef WIN32_MEMORY_TEST_H
#define WIN32_MEMORY_TEST_H

#include <windows.h>
#include "../src/win32/win32.h"
#include "../src/memory.h"

#define PAGE_SIZE 4096

static void Win32_Alloc_allocates_a_given_amount_of_memory(void **state)
{
	int32_t size = 245760;
	globalWin32State.memSentinal.prev = &globalWin32State.memSentinal;
	globalWin32State.memSentinal.next = &globalWin32State.memSentinal;

	struct platform_memory_block *test = Win32_Alloc(size, 0);

	assert_int_equal(test->size, size);

	VirtualFree(test, 0, MEM_RELEASE);
}

static void Win32_Alloc_rounds_up_on_overflow(void **state)
{
	int32_t size = PAGE_SIZE;
	globalWin32State.memSentinal.prev = &globalWin32State.memSentinal;
	globalWin32State.memSentinal.next = &globalWin32State.memSentinal;

	struct platform_memory_block *test = Win32_Alloc(size, PLATFORM_OVERFLOW);
	*((uint8_t *)test + PAGE_SIZE) = 1;

	assert_int_equal(*(uint32_t *)test->base, 1);

	VirtualFree(test, 0, MEM_RELEASE);
}

static void Win32_Alloc_rounds_down_on_underflow(void **state)
{
	int32_t size = PAGE_SIZE;
	globalWin32State.memSentinal.prev = &globalWin32State.memSentinal;
	globalWin32State.memSentinal.next = &globalWin32State.memSentinal;

	struct platform_memory_block *test = Win32_Alloc(size, PLATFORM_UNDERFLOW);
	*((uint8_t *)test + PAGE_SIZE * 2) = 1;

	assert_int_equal(*(uint32_t *)test->base, 1);

	VirtualFree(test, 0, MEM_RELEASE);
}

static void Win32_Alloc_creates_additional_space_after_generic_block_for_win32_block(void **state)
{
	int32_t size = PAGE_SIZE;
	globalWin32State.memSentinal.prev = &globalWin32State.memSentinal;
	globalWin32State.memSentinal.next = &globalWin32State.memSentinal;

	struct win32_memory_block *test = (struct win32_memory_block *)Win32_Alloc(size, 0);

	assert_true(test->next == &globalWin32State.memSentinal);

	VirtualFree(test, 0, MEM_RELEASE);
}

#endif