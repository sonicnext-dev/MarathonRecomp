#include <stdafx.h>
#include "memory.h"

constexpr size_t DEFAULT_HOST_MEMORY_BEGIN = 0x100000000;

constexpr size_t PPC_MEMORY_BEGIN = 0x20000;
constexpr size_t PPC_RESERVED_BEGIN = 0x7FEA0000;
constexpr size_t PPC_RESERVED_END = 0xA0000000;

Memory::Memory()
{
#ifdef _WIN32
    base = (uint8_t*)VirtualAlloc((void*)DEFAULT_HOST_MEMORY_BEGIN, PPC_MEMORY_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (base == nullptr)
        base = (uint8_t*)VirtualAlloc(nullptr, PPC_MEMORY_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (base == nullptr)
        return;

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    DWORD oldProtect;
    VirtualProtect(base, sysInfo.dwPageSize, PAGE_NOACCESS, &oldProtect);
#else
    base = (uint8_t*)mmap((void*)DEFAULT_HOST_MEMORY_BEGIN, PPC_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

    if (base == (uint8_t*)MAP_FAILED)
        base = (uint8_t*)mmap(nullptr, PPC_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

    if (base == nullptr)
        return;

    mprotect(base, getpagesize(), PROT_NONE);
#endif

    for (size_t i = 0; PPCFuncMappings[i].guest != 0; i++)
    {
        if (PPCFuncMappings[i].host != nullptr)
            InsertFunction(PPCFuncMappings[i].guest, PPCFuncMappings[i].host);
    }

    heap = o1heapInit(Translate(PPC_MEMORY_BEGIN), PPC_RESERVED_BEGIN - PPC_MEMORY_BEGIN);
    physicalHeap = o1heapInit(Translate(PPC_RESERVED_END), PPC_MEMORY_SIZE - PPC_RESERVED_END);
}

void* Memory::Alloc(size_t size, size_t alignment, bool physical)
{
    size = std::max<size_t>(1, size);
    alignment = alignment == 0 && physical ? 0x1000 : std::max<size_t>(16, alignment);

    std::lock_guard lock(physical ? physicalMutex : mutex);

    void* ptr = o1heapAllocate(physical ? physicalHeap : heap, size + alignment);
    size_t aligned = ((size_t)ptr + alignment) & ~(alignment - 1);

    *((void**)aligned - 1) = ptr;
    *((size_t*)aligned - 2) = size + O1HEAP_ALIGNMENT;

    return (void*)aligned;
}

void Memory::Free(void* ptr, bool physical)
{
    std::lock_guard lock(physical ? physicalMutex : mutex);
    o1heapFree(physical ? physicalHeap : heap, *((void**)ptr - 1));
}

size_t Memory::Size(void* ptr)
{
    if (ptr)
        return *((size_t*)ptr - 2) - O1HEAP_ALIGNMENT; // relies on fragment header in o1heap.c

    return 0;
}

void* MmGetHostAddress(uint32_t ptr)
{
    return g_memory.Translate(ptr);
}
