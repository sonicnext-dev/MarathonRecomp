#pragma once

#include <Marathon.inl>
#include <Sonicteam/Heap.h>
#include <Sonicteam/System/Singleton.h>
#include <Sonicteam/System/CreateStatic.h>

namespace Sonicteam
{
    class SpanverseHeap : public Heap, System::Singleton<SpanverseHeap, 0x82D3C63C, System::CreateStatic<SpanverseHeap, 0x82659A10>>
    {
    };
}
