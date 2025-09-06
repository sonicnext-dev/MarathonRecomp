#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Quaternion.h>

namespace Sonicteam::Player
{
    class ICollisionListener
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0xC);
        be<uint32_t> m_Flag1;
        be<uint32_t> m_Flag2;
        be<uint32_t> m_Flag3;
        MARATHON_INSERT_PADDING(0x4);
        SoX::Math::Vector m_CollisionNormal;
    };
}
