#pragma once
#ifndef PLATFORM_MEMORY_TEST_H
#define PLATFORM_MEMORY_TEST_H

#include <stdint.h>

#include "../src/platform.h"
#include "../src/math.h"
#include "../src/memory.h"
#include "../src/utils/utils.h"
#include "../src/win32/win32.h"

struct test_struct_mem {
	uint64_t a;
	uint32_t b;
	uint16_t c;
};

inline void ZERO_STRUCT__sets_all_bytes_in_a_struct_to_zero(void **state)
{
	struct test_struct_mem test = {
		.a = 0xDDDDDDDDDDDDDDDD,
		.b = 0xDDDDDDDD,
		.c = 0xDDDD,
	};

	ZERO_STRUCT(test);

	size_t size = sizeof(test);
	for(int32_t i = 0; i < size; ++i) {
		assert_int_equal(0, *(((uint8_t *)&test) + i));
	}
}

inline void ZERO_ARRAY__sets_all_bytes_in_an_array_to_zero(void **state)
{
	struct test_struct_mem test = {
		.a = 0xDDDDDDDDDDDDDDDD,
		.b = 0xDDDDDDDD,
		.c = 0xDDDD,
	};
	struct test_struct_mem testArray[3] = {[0 ... 2] = test};

	ZERO_ARRAY(ARRAY_COUNT(testArray), testArray);

	size_t size = sizeof(testArray);
	for(int32_t i = 0; i < size; ++i) {
		assert_int_equal(0, *(((uint8_t *)testArray) + i));
	}
}

inline void GetAlignOffset__returns_the_correct_offset_for_an_arena(void **state)
{
	uint8_t *testBuffer = malloc(4012);
	struct memory_block block = {
		.base = testBuffer,
		.used = 2007
	};
	struct memory_arena arena = {
		.currentBlock = &block
	};

	size_t result = GetAlignOffset(&arena, 4);

	assert_int_equal(1, result);

	free(testBuffer);
}

inline void PUSH_STRUCT__returns_plat_block(void **state)
{
	api.Alloc = Win32_Alloc;

	uint8_t *testBuffer = malloc(4012);
	struct memory_block block = {
		.base = testBuffer,
		.used = 2008
	};
	struct memory_arena arena = {
		.currentBlock = &block
	};

	struct test_struct_mem *test = PUSH_STRUCT(&arena, struct test_struct_mem, DEF_PUSH);

	assert_int_equal(sizeof(struct test_struct_mem), arena.used);

	free(testBuffer);
}

#endif