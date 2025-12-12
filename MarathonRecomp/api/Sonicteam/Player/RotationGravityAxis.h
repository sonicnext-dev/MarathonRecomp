#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPosturePlugIn.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Player
{
    class RotationGravityAxis : public IPosturePlugIn
    {
    public:
        SoX::Math::Vector m_GravityAxis;
    };

    MARATHON_ASSERT_SIZEOF(RotationGravityAxis, 0x20);
    MARATHON_ASSERT_OFFSETOF(RotationGravityAxis, m_GravityAxis, 0x10);
}
