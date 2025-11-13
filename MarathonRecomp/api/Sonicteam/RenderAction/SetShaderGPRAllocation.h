#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetShaderGPRAllocation : public MyRenderProcess
    {
    public:
        be<uint32_t> m_VertexShaderCount;
        be<uint32_t> m_PixelShaderCount;
    };

    MARATHON_ASSERT_OFFSETOF(SetShaderGPRAllocation, m_VertexShaderCount, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetShaderGPRAllocation, m_PixelShaderCount, 0x3C);
    MARATHON_ASSERT_SIZEOF(SetShaderGPRAllocation, 0x40);
}
