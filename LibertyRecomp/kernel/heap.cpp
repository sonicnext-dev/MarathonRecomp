#include <stdafx.h>
#include "heap.h"
#include "memory.h"
#include "function.h"
#include "xdm.h"

// XMA I/O only needs 64KB but we keep some reserved space for safety
// Shrink reserved region to give more physical memory to the game
// Original: RESERVED_END = 0xA0000000 (1.5GB physical)
// New: RESERVED_END = 0x80000000 (2GB physical - from 0x80000000 to 0x100000000)
constexpr size_t RESERVED_BEGIN = 0x7FEA0000;
constexpr size_t RESERVED_END = 0x80000000;  // Was 0xA0000000

void Heap::Init()
{
    heap = o1heapInit(g_memory.Translate(0x20000), RESERVED_BEGIN - 0x20000);
    physicalHeap = o1heapInit(g_memory.Translate(RESERVED_END), 0x100000000 - RESERVED_END);
}

void* Heap::Alloc(size_t size)
{
    std::lock_guard lock(mutex);

    void* ptr = o1heapAllocate(heap, std::max<size_t>(1, size));
    if (ptr == nullptr)
    {
        const O1HeapDiagnostics diag = o1heapGetDiagnostics(heap);
        std::fprintf(stderr,
            "[Heap] OOM (heap): request=%zu capacity=%zu allocated=%zu peak_allocated=%zu peak_request=%zu oom_count=%llu\n",
            size,
            diag.capacity,
            diag.allocated,
            diag.peak_allocated,
            diag.peak_request_size,
            static_cast<unsigned long long>(diag.oom_count));
        std::fflush(stderr);
    }

    return ptr;
}

void* Heap::AllocPhysical(size_t size, size_t alignment)
{
    size = std::max<size_t>(1, size);
    alignment = alignment == 0 ? 0x1000 : std::max<size_t>(16, alignment);

    std::lock_guard lock(physicalMutex);

    void* ptr = o1heapAllocate(physicalHeap, size + alignment);
    if (ptr == nullptr)
    {
        const O1HeapDiagnostics diag = o1heapGetDiagnostics(physicalHeap);
        std::fprintf(stderr,
            "[Heap] OOM (physical): request=%zu align=%zu capacity=%zu allocated=%zu peak_allocated=%zu peak_request=%zu oom_count=%llu\n",
            size,
            alignment,
            diag.capacity,
            diag.allocated,
            diag.peak_allocated,
            diag.peak_request_size,
            static_cast<unsigned long long>(diag.oom_count));
        std::fflush(stderr);
        return nullptr;
    }
    size_t aligned = ((size_t)ptr + alignment) & ~(alignment - 1);

    *((void**)aligned - 1) = ptr;
    *((size_t*)aligned - 2) = size + O1HEAP_ALIGNMENT;

    return (void*)aligned;
}

void Heap::Free(void* ptr)
{
    if (ptr >= physicalHeap)
    {
        std::lock_guard lock(physicalMutex);
        o1heapFree(physicalHeap, *((void**)ptr - 1));
    }
    else
    {
        std::lock_guard lock(mutex);
        o1heapFree(heap, ptr);
    }
}

size_t Heap::Size(void* ptr)
{
    if (ptr)
        return *((size_t*)ptr - 2) - O1HEAP_ALIGNMENT; // relies on fragment header in o1heap.c

    return 0;
}

uint32_t RtlAllocateHeap(uint32_t heapHandle, uint32_t flags, uint32_t size)
{
    void* ptr = g_userHeap.Alloc(size);
    if ((flags & 0x8) != 0)
        memset(ptr, 0, size);

    assert(ptr);
    return g_memory.MapVirtual(ptr);
}

uint32_t RtlReAllocateHeap(uint32_t heapHandle, uint32_t flags, uint32_t memoryPointer, uint32_t size)
{
    void* ptr = g_userHeap.Alloc(size);
    if ((flags & 0x8) != 0)
        memset(ptr, 0, size);

    if (memoryPointer != 0)
    {
        void* oldPtr = g_memory.Translate(memoryPointer);
        memcpy(ptr, oldPtr, std::min<size_t>(size, g_userHeap.Size(oldPtr)));
        g_userHeap.Free(oldPtr);
    }

    assert(ptr);
    return g_memory.MapVirtual(ptr);
}

uint32_t RtlFreeHeap(uint32_t heapHandle, uint32_t flags, uint32_t memoryPointer)
{
    if (memoryPointer != NULL)
        g_userHeap.Free(g_memory.Translate(memoryPointer));

    return true;
}

uint32_t RtlSizeHeap(uint32_t heapHandle, uint32_t flags, uint32_t memoryPointer)
{
    if (memoryPointer != NULL)
        return (uint32_t)g_userHeap.Size(g_memory.Translate(memoryPointer));

    return 0;
}

uint32_t XAllocMem(uint32_t size, uint32_t flags)
{
    void* ptr = (flags & 0x80000000) != 0 ?
        g_userHeap.AllocPhysical(size, (1ull << ((flags >> 24) & 0xF))) :
        g_userHeap.Alloc(size);

    if ((flags & 0x40000000) != 0)
        memset(ptr, 0, size);

    assert(ptr);
    return g_memory.MapVirtual(ptr);
}

void XFreeMem(uint32_t baseAddress, uint32_t flags)
{
    if (baseAddress != NULL)
        g_userHeap.Free(g_memory.Translate(baseAddress));
}

uint32_t XVirtualAlloc(void *lpAddress, unsigned int dwSize, unsigned int flAllocationType, unsigned int flProtect)
{
    assert(!lpAddress);
    return g_memory.MapVirtual(g_userHeap.Alloc(dwSize));
}

uint32_t XVirtualFree(uint32_t lpAddress, unsigned int dwSize, unsigned int dwFreeType)
{
    if ((dwFreeType & 0x8000) != 0 && dwSize)
        return FALSE;

    if (lpAddress)
        g_userHeap.Free(g_memory.Translate(lpAddress));

    return TRUE;
}

GUEST_FUNCTION_HOOK(sub_82915668, XVirtualAlloc);
GUEST_FUNCTION_HOOK(sub_829156B8, XVirtualFree);

GUEST_FUNCTION_STUB(sub_82535588); // HeapCreate // replaced
// GUEST_FUNCTION_STUB(sub_82BD9250); // HeapDestroy

GUEST_FUNCTION_HOOK(sub_82535B38, RtlAllocateHeap); // repalced
GUEST_FUNCTION_HOOK(sub_82536420, RtlFreeHeap); // replaced
GUEST_FUNCTION_HOOK(sub_82536708, RtlReAllocateHeap); // replaced
GUEST_FUNCTION_HOOK(sub_82534DD0, RtlSizeHeap); // replaced

GUEST_FUNCTION_HOOK(sub_82537E70, XAllocMem); // replaced
GUEST_FUNCTION_HOOK(sub_82537F08, XFreeMem); // replaced
