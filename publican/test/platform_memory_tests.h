#pragma once
#ifndef PLATFORM_MEMORY_TEST_H
#define PLATFORM_MEMORY_TEST_H

#include <stdint.h>

#include "../src/memory.h"
#include "../src/utils/utils.h"

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

#endif