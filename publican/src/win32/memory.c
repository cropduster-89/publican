#include <assert.h>

#include "../platform.h"
#include "../memory.h"
#include "../intrinsics.h"
#include "../maths.h"
#include "win32.h"

static void FreeBlock(struct win32_memory_block *block)
{
	BeginTicketMutex(&globalWin32State.memMutex);
	block->prev->next = block->next;
	block->next->prev = block->prev;
	EndTicketMutex(&globalWin32State.memMutex);

	BOOL result = VirtualFree(block, 0, MEM_RELEASE);
	assert(result);
}

extern void Win32_DeAlloc(struct platform_memory_block *block)
{
	if(block) {
		struct win32_memory_block *win32Block = ((struct win32_memory_block *)block);
		FreeBlock(win32Block);
	}
}

extern struct platform_memory_block *Win32_Alloc(size_t size,
												 DWORDLONG flags)
{
	assert(sizeof(struct win32_memory_block) == 64);
#define PAGE_SIZE 4096
	uintptr_t pageSize = PAGE_SIZE;
	uintptr_t totalSize = size + sizeof(struct win32_memory_block);
	uintptr_t baseOffset = sizeof(struct win32_memory_block);
	uintptr_t protectOffset = 0;

	if(flags & PLATFORM_UNDERFLOW) {
		totalSize = pageSize * 2 + size;
		baseOffset = 2 * pageSize;
		protectOffset = pageSize;
	} else if(flags & PLATFORM_OVERFLOW) {
		uintptr_t sizeRoundUp = AlignPow2(size, pageSize);
		totalSize = sizeRoundUp + 2 * pageSize;
		baseOffset = pageSize + sizeRoundUp - size;
		protectOffset = pageSize + sizeRoundUp;
	}
	struct win32_memory_block *block = (struct win32_memory_block *)
		VirtualAlloc(0, totalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	assert(block);
	block->block.base = (BYTE *)block + baseOffset;
	assert(block->block.used == 0 && block->block.arenaPrev == 0);

	if(flags & (PLATFORM_UNDERFLOW | PLATFORM_OVERFLOW)) {
		DWORD oldProtect = 0;
		BOOL boolProtect = VirtualProtect((BYTE *)block + protectOffset,
			pageSize, PAGE_NOACCESS, &oldProtect);
		assert(boolProtect);
	}
	struct win32_memory_block *sentinal = &globalWin32State.memSentinal;
	block->next = sentinal;
	block->block.size = size;
	block->block.flags = flags;

	BeginTicketMutex(&globalWin32State.memMutex);
	block->prev = sentinal->prev;
	block->prev->next = block;
	block->next->prev = block;
	EndTicketMutex(&globalWin32State.memMutex);

	struct platform_memory_block *patBlock = &block->block;

	return(patBlock);
}