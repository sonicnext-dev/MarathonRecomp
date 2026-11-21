#pragma once

#include <Marathon.inl>
#include <Kaim/CObject.h>

namespace Kaim
{
    class IConstraint : public CObject
    {
    public:
        struct Vftable : public CObject::Vftable
        {
            be<uint32_t> fpGetClass;
            be<uint32_t> fpGetCost;
            be<uint32_t> fpGetCost_2;
        };
    };

    MARATHON_ASSERT_SIZEOF(IConstraint, 0x04);
}
