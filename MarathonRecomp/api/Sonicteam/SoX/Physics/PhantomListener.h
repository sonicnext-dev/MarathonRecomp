#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Physics
{
    class PhantomListener : public RefCountObject
    {
    public:
        struct Vftable : public RefCountObject::Vftable
        {
            MARATHON_INSERT_PADDING(0x04);
            be<uint32_t> fpOnEnter;
            be<uint32_t> fpOnLeave;
        };
    };

    MARATHON_ASSERT_SIZEOF(PhantomListener, 0x08);
}
