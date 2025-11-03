#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Audio
{
    class IAudioEngine
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(IAudioEngine, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IAudioEngine, 4);
}   
