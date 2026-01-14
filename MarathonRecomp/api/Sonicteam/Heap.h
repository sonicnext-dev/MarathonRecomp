#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class Heap
    {
        struct _HeapBlock {
            be<uint32_t> Flags;          // 0x00: Flags (e.g., is_free, block_size)
            be<uint32_t> AllocTag;       // 0x04: Allocation tag (for debugging/tracking)
            be<uint32_t> PreviousFree;   // 0x08: Pointer to the previous free block (or metadata)
            xpointer<_HeapBlock> NextFree;       // 0x0C: Pointer to the next free block
            be<uint32_t> Unused1;          // 0x10: Unused
            be<uint32_t> Unused2;          // 0x14: Unused
            be<uint32_t> Unused3;          // 0x18: Unused
            be<uint32_t> BlockSize;      // 0x1C: Total size of this block (including metadata)
            be<uint32_t> Unused4;          // 0x20: Unused
            be<uint32_t> Unused5;          // 0x24: Unused
            be<uint32_t> Unused6;          // 0x28: Unused
            be<uint32_t> Unused7;          // 0x2C: Unused
        };

        struct _HeapArea {
            be<uint32_t> Magic;            // 0x00: Magic number (e.g., 0x4C485A49)
            be<uint32_t> TotalSize;              // 0x04: Total size of the heap
            be<uint32_t> UsedSize;               // 0x08: Currently used memory
            be<uint32_t> FreeSize;               // 0x0C: Currently free memory
            be<uint32_t> Alignment;        // 0x10: Alignment requirement for allocations
            xpointer<_HeapBlock> LastAllocated;    // 0x14: Last block allocated
            xpointer <_HeapBlock> FreeListHead;      // 0x18: Head of the free block list
            be<uint32_t> AllocListHead;    // 0x1C: Head of the allocated block list
            be<uint32_t> AllocCount;       // 0x20: Number of allocations
            be<uint32_t> FreeBlockCount;   // 0x24: Number of free blocks
            be<uint32_t> AllocatedBlocks;  // 0x28: Number of allocated blocks
            be<uint32_t> Unused1;            // 0x2C: Unused/Reserved
            be<uint32_t> Unused2;
        };


        //no idea about , size  == 0x1C (void*...) i think
        struct _HeapInfo
        {

        };

    public:
        struct Vftable
        {
            be<uint32_t> fpHeapAlloc;
            be<uint32_t> fpHeapFree;
            be<uint32_t> fpHeapInfo;
            be<uint32_t> fpGetHeapSize;
            be<uint32_t> fpGetHeapAligment;
            be<uint32_t> fpIsHeapFull; // i think
            be<uint32_t> fpGetHeapStart;
            be<uint32_t> fpGetHeapEnd;
            be<uint32_t> fpGetHeapArea;
            be<uint32_t> fpHeapAllocMeta; // with meta-data
        };

        xpointer<Vftable> m_pVftable;
        xpointer<_HeapArea> m_pHeapArea; // HeapArea, (HEADER data = ) (LHZI) 0xC
        be<uint32_t> m_HeapAreaFlag;
        be<uint32_t> m_heap_last_rng; //  //   v8 ^= srand_ithink(0xFFu); =  heap_last_rng = v8, durring alloc/dealloc
        char m_Field10;
        char m_Field11;
        size_t m_HeapPadding; //0x14, or size == 0x20 , why 0x20?, BlockSize?
    };

}
