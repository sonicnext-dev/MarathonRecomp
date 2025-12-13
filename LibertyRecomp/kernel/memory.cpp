#include <stdafx.h>
#include "memory.h"

static constexpr size_t AlignDown(size_t value, size_t alignment) noexcept
{
    return value & ~(alignment - 1);
}

static constexpr size_t AlignUp(size_t value, size_t alignment) noexcept
{
    return (value + (alignment - 1)) & ~(alignment - 1);
}

Memory::Memory()
{
#ifdef _WIN32
    base = (uint8_t*)VirtualAlloc((void*)0x100000000ull, PPC_MEMORY_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (base == nullptr)
        base = (uint8_t*)VirtualAlloc(nullptr, PPC_MEMORY_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (base == nullptr)
        return;

    // Some titles (e.g., GTA IV) legitimately touch low memory (including address 0).
    // Do not install a null-page guard in that case.
#else
    base = (uint8_t*)mmap((void*)0x100000000ull, PPC_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

    if (base == (uint8_t*)MAP_FAILED)
        base = (uint8_t*)mmap(NULL, PPC_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

    if (base == (uint8_t*)MAP_FAILED)
    {
        base = nullptr;
        return;
    }

    // Some titles (e.g., GTA IV) legitimately touch low memory (including address 0).
    // Do not install a null-page guard in that case.
#endif

    for (size_t i = 0; PPCFuncMappings[i].guest != 0; i++)
    {
        if (PPCFuncMappings[i].host != nullptr)
            InsertFunction(PPCFuncMappings[i].guest, PPCFuncMappings[i].host);
    }

    // Protect the recomp function lookup table from guest writes.
    // If the game overwrites these host function pointers, it can lead to crashes that look like
    // invalid indirect branches / pointer-authentication failures on arm64e.
    constexpr size_t kPageSize = 0x1000;
    constexpr size_t kFuncTableOffset = PPC_IMAGE_BASE + PPC_IMAGE_SIZE;
    constexpr size_t kFuncTableSize = (PPC_CODE_SIZE * 2) + sizeof(PPCFunc*);
    const size_t protectBegin = AlignDown(kFuncTableOffset, kPageSize);
    const size_t protectEnd = AlignUp(kFuncTableOffset + kFuncTableSize, kPageSize);
    if (protectEnd > protectBegin)
    {
#ifdef _WIN32
        DWORD oldProtect{};
        VirtualProtect(base + protectBegin, protectEnd - protectBegin, PAGE_READONLY, &oldProtect);
#else
        mprotect(base + protectBegin, protectEnd - protectBegin, PROT_READ);
#endif
    }
}

void* MmGetHostAddress(uint32_t ptr)
{
    return g_memory.Translate(ptr);
}
