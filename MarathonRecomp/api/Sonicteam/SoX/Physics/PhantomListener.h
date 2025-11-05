#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Physics
{
    class PhantomListener : public RefCountObject {};

    MARATHON_ASSERT_SIZEOF(PhantomListener, 8);
}
