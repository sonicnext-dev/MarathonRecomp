#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class Rasterize : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x4);
        xpointer<SoX::Graphics::Texture> m_pTexture;
        bool m_Flag;
    };

    MARATHON_ASSERT_SIZEOF(Rasterize, 0x44);
    MARATHON_ASSERT_OFFSETOF(Rasterize, m_pTexture, 0x3C);
    MARATHON_ASSERT_OFFSETOF(Rasterize, m_Flag, 0x40);
}
