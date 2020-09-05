#pragma once
#ifndef PUB_UTILS_H
#define PUB_UTILS_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#define INVALID_PATH assert(0)
#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))

inline uint32_t SafeTruncateUInt64(uint64_t a)
{
	assert(a <= 0xFFFFFFFF);
	uint32_t result = (uint32_t)a;
	return(result);
}

inline uint16_t SafeTruncateUInt16(uint32_t a)
{
	assert(a <= 0xFFFF);
	uint16_t result = (uint16_t)a;
	return(result);
}

bool IsLineEnd(char c);
bool IsWhitespace(char c);
bool StringsAreEqual(uintptr_t length, char *a, char *b);

#endif