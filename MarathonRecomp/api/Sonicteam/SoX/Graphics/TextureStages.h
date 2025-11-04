#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Graphics
{
    class TextureStages
    {
    public:
        xpointer<void> m_pVftable;
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
    };

    MARATHON_ASSERT_SIZEOF(TextureStages, 0x8);
}
