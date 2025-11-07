#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetStencilWriteMask : public MyRenderProcess
    {
    public:
        be<uint32_t> m_WriteMask;
    };

    MARATHON_ASSERT_SIZEOF(SetStencilWriteMask, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetStencilWriteMask, m_WriteMask, 0x38);
}
