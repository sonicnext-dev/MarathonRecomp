#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetCSMTextures : public MyRenderProcess
    {
    public:
        bool m_Flag;
        xpointer<SoX::Graphics::Texture> m_pCSMTexture;
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_OFFSETOF(SetCSMTextures, m_Flag, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetCSMTextures, m_pCSMTexture, 0x3C);
    MARATHON_ASSERT_SIZEOF(SetCSMTextures, 0x44);
}
