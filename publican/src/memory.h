#pragma once
#ifndef PUB_MEMORY_H
#define PUB_MEMORY_H

#include <stdbool.h>
#include <stdint.h>

enum memory_block_flags {
	PLATFORM_NOTRESTORED = 0x1,
	PLATFORM_OVERFLOW = 0x2,
	PLATFORM_UNDERFLOW = 0x4
};

struct memory_block {
	enum memory_block_flags flags;

	uint64_t size;
	uint8_t *base;
	uintptr_t used;

	struct memory_block *arenaPrev;
};

struct memory_arena {
	struct memory_block *currentBlock;
	uintptr_t minBlockSize;

	uint64_t allocationFlags;
	int32_t tempCount;
};

struct temp_memory {
	struct memory_arena *arena;
	struct memory_block *block;
	uintptr_t used;
};

#define ZERO_STRUCT(instance) ZeroSize_(sizeof(instance), &(instance))
#define ZERO_ARRAY(count, ptr) ZeroSize_(count * sizeof((ptr)[0]), ptr)
inline void ZeroSize_(size_t size, void *ptr)
{
	uint8_t *byte = (uint8_t *)ptr;
	while(size--) {
		*byte++ = 0;
	}
}

#endif