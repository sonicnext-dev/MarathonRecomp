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
        be<float> c_gauge_max;
        be<float> c_gauge_green;
        be<float> c_gauge_red;
        be<float> c_gauge_blue;
        be<float> c_gauge_white;
        be<float> c_gauge_sky;
        be<float> c_gauge_yellow;
        be<float> c_gauge_purple;
        be<float> c_gauge_super;
        be<float> c_gauge_heal;
        be<float> c_gauge_heal_delay;
    };
}
