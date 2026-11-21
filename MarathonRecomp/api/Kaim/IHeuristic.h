#pragma once

#include <Marathon.inl>
#include <Kaim/CObject.h>

namespace Kaim
{
    class IHeuristic : public CObject
    {
    public:
        struct Vftable : public CObject::Vftable
        {
            be<uint32_t> fpGetClass;
            be<uint32_t> fpGetCost;
        };
    };

    MARATHON_ASSERT_SIZEOF(IHeuristic, 0x04);
}
