#pragma once
#ifndef MATHS_TEST_H
#define MATHS_TEST_H

#include "../src/maths.h"
#include <intrin.h>

#define PAGE_SIZE 4096

inline void MULT_VEC_can_multiply_a_vector(void **state)
{
	union vec4 a = {.x = 1, .y = 1, .z = 1, .w = 1};

	union vec4 result = MULT_VEC(a, 2);

	assert_int_equal(result.x, 2);
}

#endif