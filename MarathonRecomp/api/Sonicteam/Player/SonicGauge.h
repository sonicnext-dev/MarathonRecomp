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

    MARATHON_ASSERT_OFFSETOF(SonicGauge, m_Value, 0x28);
    MARATHON_ASSERT_OFFSETOF(SonicGauge, m_GroundedTime, 0x2C);
    MARATHON_ASSERT_OFFSETOF(SonicGauge, m_Flags, 0x30);
    MARATHON_ASSERT_OFFSETOF(SonicGauge, m_GroundedFlags, 0x34);
    MARATHON_ASSERT_OFFSETOF(SonicGauge, c_gauge_max, 0x38);
    MARATHON_ASSERT_OFFSETOF(SonicGauge, c_gauge_green, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SonicGauge, c_gauge_red, 0x40);
    MARATHON_ASSERT_OFFSETOF(SonicGauge, c_gauge_blue, 0x44);
    MARATHON_ASSERT_OFFSETOF(SonicGauge, c_gauge_white, 0x48);
    MARATHON_ASSERT_OFFSETOF(SonicGauge, c_gauge_sky, 0x4C);
    MARATHON_ASSERT_OFFSETOF(SonicGauge, c_gauge_yellow, 0x50);
    MARATHON_ASSERT_OFFSETOF(SonicGauge, c_gauge_purple, 0x54);
    MARATHON_ASSERT_OFFSETOF(SonicGauge, c_gauge_super, 0x58);
    MARATHON_ASSERT_OFFSETOF(SonicGauge, c_gauge_heal, 0x5C);
    MARATHON_ASSERT_OFFSETOF(SonicGauge, c_gauge_heal_delay, 0x60);
    MARATHON_ASSERT_SIZEOF(SonicGauge, 0x64);
}
