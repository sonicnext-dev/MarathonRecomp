#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class SonicGauge : public IGauge
    {
    public:
        be<float> m_Value;
        be<float> m_GroundedTime;
        be<uint32_t> m_Flags;
        be<uint32_t> m_GroundedFlags;
        be<float> m_Maximum;
        MARATHON_INSERT_PADDING(0x28);
    };
}
