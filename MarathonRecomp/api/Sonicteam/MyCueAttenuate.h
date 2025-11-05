#pragma once

class MyCueAttenuate
{
public:
    be<float> m_Time;
    be<float> m_FadeOutRate;
    be<float> m_FadeInRate;
    be<float> m_MinVolume;
    be<float> m_MaxVolume;
    bool m_IsFadeOut;
    MARATHON_INSERT_PADDING(3);
};

MARATHON_ASSERT_OFFSETOF(MyCueAttenuate, m_Time, 0x00);
MARATHON_ASSERT_OFFSETOF(MyCueAttenuate, m_FadeOutRate, 0x04);
MARATHON_ASSERT_OFFSETOF(MyCueAttenuate, m_FadeInRate, 0x08);
MARATHON_ASSERT_OFFSETOF(MyCueAttenuate, m_MinVolume, 0x0C);
MARATHON_ASSERT_OFFSETOF(MyCueAttenuate, m_MaxVolume, 0x10);
MARATHON_ASSERT_OFFSETOF(MyCueAttenuate, m_IsFadeOut, 0x14);
MARATHON_ASSERT_SIZEOF(MyCueAttenuate, 0x18);
