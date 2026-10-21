#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::PathAnimation
{
    class Animation : public SoX::RefCountObject {};

    MARATHON_ASSERT_SIZEOF(Animation, 0x8);
}
