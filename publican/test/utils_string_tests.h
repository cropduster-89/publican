#pragma once
#ifndef UTILS_STRING_TEST_H
#define UTILS_STRING_TEST_H

#include <stdbool.h>

#include "../src/utils/utils.h"

inline void IsLineEnd_can_identify_newline(void **state)
{
	bool result = IsLineEnd('\n');

	assert_true(result);
}

inline void IsLineEnd_can_identify_non_newline(void **state)
{
	bool result = IsLineEnd('a');

	assert_false(result);
}

inline void IsWhitespace_can_identify_whitespace(void **state)
{
	bool result = IsWhitespace('\t');

	assert_true(result);
}

inline void IsWhitespace_can_identify_non_whitespace(void **state)
{
	bool result = IsWhitespace('a');

	assert_false(result);
}

inline void StringsAreEqual_can_identify_same_string(void **state)
{
	bool result = StringsAreEqual(4, "test", "test");

	assert_true(result);
}

inline void StringsAreEqual_can_identify_different_string(void **state)
{
	bool result = StringsAreEqual(4, "test", "tset");

	assert_false(result);
}

#endif