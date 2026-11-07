#pragma once

#include <Marathon.inl>
#include <Chao/CSD/Core/csdMemoryAlloc.h>

namespace Sonicteam
{
    class CMemoryAllocMarathon : public Chao::CSD::CMemoryAlloc {};

    MARATHON_ASSERT_SIZEOF(CMemoryAllocMarathon, 0x04);
}
