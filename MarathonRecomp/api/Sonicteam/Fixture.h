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
        Sonicteam::SoX::Math::Quaternion m_Rotation;
        Sonicteam::SoX::Math::Vector m_Position;
        MARATHON_INSERT_PADDING(0xF0);
    };
}
