#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class ICollisionListener
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0xC);
        be<uint32_t> m_FlagsA;
        be<uint32_t> m_FlagsB;
        be<uint32_t> m_FlagsC;
        MARATHON_INSERT_PADDING(0x4);
        SoX::Math::Vector m_SurfaceNormal;
    };
}
