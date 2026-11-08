#pragma once

#include <Sonicteam/SoX/Math/Quaternion.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::SoX::Math
{
    class Transform
    {
    public:
        Quaternion m_Rotation;
        Vector m_Position;
    };

    MARATHON_ASSERT_OFFSETOF(Transform, m_Rotation, 0x00);
    MARATHON_ASSERT_OFFSETOF(Transform, m_Position, 0x10);
    MARATHON_ASSERT_SIZEOF(Transform, 0x20);
}
