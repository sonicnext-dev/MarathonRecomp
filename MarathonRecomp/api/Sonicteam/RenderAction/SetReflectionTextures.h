#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetReflectionTextures : public MyRenderProcess
    {
    public:
        bool m_Flag;
        xpointer<void> m_pReflectionTexture;
    };

    MARATHON_ASSERT_SIZEOF(SetReflectionTextures, 0x40);
    MARATHON_ASSERT_OFFSETOF(SetReflectionTextures, m_Flag, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetReflectionTextures, m_pReflectionTexture, 0x3C);
}
