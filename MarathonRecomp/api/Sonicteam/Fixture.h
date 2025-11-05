#pragma once

#include <Marathon.inl>
#include <Sonicteam/Actor.h>
#include <Sonicteam/SoX/Math/Quaternion.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam
{
    class Fixture : public Actor
    {
    public:
        SoX::Math::Quaternion m_Rotation;
        SoX::Math::Vector m_Position;
        MARATHON_INSERT_PADDING(0xF0);
    };

    MARATHON_ASSERT_OFFSETOF(Fixture, m_Rotation, 0x60);
    MARATHON_ASSERT_OFFSETOF(Fixture, m_Position, 0x70);
    MARATHON_ASSERT_SIZEOF(Fixture, 0x170);
}
