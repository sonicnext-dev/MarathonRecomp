#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::Prop
{
    class EntityHandle : public SoX::RefCountObject
    {
    public:
        MARATHON_INSERT_PADDING(0x14);
    };

    MARATHON_ASSERT_SIZEOF(EntityHandle, 0x1C);
}
