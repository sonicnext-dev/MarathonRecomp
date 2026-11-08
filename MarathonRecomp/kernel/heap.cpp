#include <stdafx.h>
#include "heap.h"
#include "memory.h"
#include "function.h"

#define X_ALLOC_SHIFT_ALIGNMENT_BITS 24
#define X_ALLOC_MASK_ALIGNMENT_BITS 0xF
#define X_ALLOC_FLAG_PHYSICAL 0x80000000

static void* Guest_XAllocMem(uint32_t size, uint32_t flags)
{
    return GuestToHostFunction<void*>(sub_82537E70, size, flags);
}

static void Guest_XFreeMem(void* ptr, uint32_t flags)
{
    GuestToHostFunction<void>(sub_82537F08, ptr, flags);
}

void* Heap::Alloc(size_t size)
{
    return Guest_XAllocMem(size, 0);
}

void* Heap::AllocPhysical(size_t size, size_t alignment)
{
    uint32_t alignBit = alignment ? __builtin_ctz(alignment) : 0;
    uint32_t alignFlag = (alignBit & X_ALLOC_MASK_ALIGNMENT_BITS) << X_ALLOC_MASK_ALIGNMENT_BITS;

    return Guest_XAllocMem(size, X_ALLOC_FLAG_PHYSICAL | alignFlag);
}

void Heap::Free(void* ptr)
{
    Guest_XFreeMem(ptr, ptr >= g_memory.physicalHeap ? X_ALLOC_FLAG_PHYSICAL : 0);
}
