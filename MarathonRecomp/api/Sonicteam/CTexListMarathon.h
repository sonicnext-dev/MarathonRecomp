#pragma once

#include <Marathon.inl>
#include <Chao/CSD/Core/csdTexList.h>

namespace Sonicteam
{
    class CTexListMarathon : public Chao::CSD::CTexList {};

    MARATHON_ASSERT_SIZEOF(CTexListMarathon, 0x20);
}
