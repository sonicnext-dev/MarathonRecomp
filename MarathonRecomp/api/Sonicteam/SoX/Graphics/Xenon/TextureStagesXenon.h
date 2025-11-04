#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/TextureStages.h>

class MyGraphicsDevice;

namespace Sonicteam::SoX::Graphics::Xenon
{
    class TextureStagesXenon : public TextureStages
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(0x14);
        };

        xpointer<Vftable> m_pVftable;
        MARATHON_INSERT_PADDING(0x13B);
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
        MARATHON_INSERT_PADDING(0x48);
    };

    MARATHON_ASSERT_SIZEOF(TextureStagesXenon, 0x194);
    MARATHON_ASSERT_OFFSETOF(TextureStagesXenon, m_pMyGraphicsDevice, 0x148);
}
