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
