#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetDepthTextures : public MyRenderProcess
    {
    public:
        bool m_Flag;
        xpointer<SoX::Graphics::Texture> m_pDepthTexture;
    };

    MARATHON_ASSERT_OFFSETOF(SetDepthTextures, m_Flag, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetDepthTextures, m_pDepthTexture, 0x3C);
    MARATHON_ASSERT_SIZEOF(SetDepthTextures, 0x40);
}
