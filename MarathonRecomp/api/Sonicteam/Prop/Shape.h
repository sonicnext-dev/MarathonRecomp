#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::Prop
{
    class Shape : public SoX::RefCountObject {};

    MARATHON_ASSERT_SIZEOF(Shape, 0x08);
}
