#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Audio
{
    class Listener : public RefCountObject
    {
    public:
        MARATHON_INSERT_PADDING(0x38);
    };

    MARATHON_ASSERT_SIZEOF(Listener, 0x40);
}
