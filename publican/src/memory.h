#pragma once
#ifndef PUB_MEMORY_H
#define PUB_MEMORY_H

#include <stdbool.h>
#include <stdint.h>

#include "maths.h"

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

struct arena_push_params {
		uint32_t flags;
		uint32_t align;
};

struct memory_arena {
	struct platform_memory_block *currentBlock;
	uintptr_t minBlockSize;

	uint64_t allocationFlags;
	int32_t tempCount;
};

#define DEF_PUSH (_DefaultPushParams())
inline struct arena_push_params _DefaultPushParams(void)
{
	struct arena_push_params params = {};
	params.flags = 0x1;
	params.align = 4;
	return(params);
}

struct arena_boot_params {
	uint64_t allocationFlags;
	uintptr_t minBlockSize;
};

#define DEF_BOOT (_DefaultBootParams())
inline struct arena_boot_params _DefaultBootParams(void)
{
	struct arena_boot_params params = {};
	return(params);
}

struct temp_memory {
	struct memory_arena *arena;
	struct platform_memory_block *block;
	uintptr_t used;
};

#define ZERO_STRUCT(instance) _ZeroSize(sizeof(instance), &(instance))
#define ZERO_ARRAY(count, ptr) _ZeroSize(count * sizeof((ptr)[0]), ptr)
inline void _ZeroSize(size_t size,
					  void *ptr)
{
	uint8_t *byte = (uint8_t *)ptr;
	while(size--) {
		*byte++ = 0;
	}
}

inline size_t GetAlignOffset(struct memory_arena *arena,
							 size_t align)
{
	size_t alignOffset = 0;

	size_t resultPtr = (size_t)arena->currentBlock->base + arena->currentBlock->used;
	size_t alignMask = align - 1;
	if(resultPtr & alignMask) {
			alignOffset = align - (resultPtr & alignMask);
	}

	return(alignOffset);
}

inline size_t GetEffectiveSizeFor(struct memory_arena *arena,
								  size_t sizeInit,
								  struct arena_push_params params)
{
	size_t size = sizeInit;

	size_t alignOffset = GetAlignOffset(arena, params.align);
	size += alignOffset;

	return(size);
}

#define PUSH_STRUCT(arena, type, params) \
	(type *)_PushSize(arena, sizeof(type), params)
inline void *_PushSize(struct memory_arena *arena,
					   size_t sizeInit,
					   struct arena_push_params params)
{
	void *result = 0;
	size_t size = 0;
	if(arena->currentBlock) {
		size = GetEffectiveSizeFor(arena, sizeInit, params);
	}
	if(!arena->currentBlock || (arena->currentBlock->used + size)
		> arena->currentBlock->size) {
		size = sizeInit;
		if(arena->allocationFlags & (PLATFORM_OVERFLOW | PLATFORM_UNDERFLOW)) {
			arena->minBlockSize = 0;
			size = AlignPow2(size, params.align);
		} else if(!arena->minBlockSize) {
			arena->minBlockSize = 1024 * 1024;
		}
		size_t blockSize = MAX(size, arena->minBlockSize);

		struct memory_block *newBlock = api.Alloc(blockSize, arena->allocationFlags);
		newBlock->arenaPrev = arena->currentBlock;
		arena->currentBlock = newBlock;
	}
	assert((arena->currentBlock->used + size) <= arena->currentBlock->size);

	size_t alignOffset = GetAlignOffset(arena, params.align);
	result = arena->currentBlock->base + arena->currentBlock->used + alignOffset;
	arena->currentBlock->used += size;

	assert(size >= sizeInit);

	if(params.flags & 0x1) {
		ZeroSize_(sizeInit, result);
	}
	return(result);
}

#endif