#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class CopyTexture : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x4);
        xpointer<void> m_pTextureXenon;
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_SIZEOF(CopyTexture, 0x44);
    MARATHON_ASSERT_OFFSETOF(CopyTexture, m_pTextureXenon, 0x3C);
}
