#pragma once

#include <Marathon.inl>
#include <hk330/hkEntity.h>

namespace hk330
{
    class hkRigidBody : public hkEntity {};

    MARATHON_ASSERT_SIZEOF(hkRigidBody, 0x26C);
}
