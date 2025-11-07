#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetShaderGPRAllocation : public MyRenderProcess
    {
    public:
        be<uint32_t> m_Field0;
        be<uint32_t> m_Field1;
    };

    MARATHON_ASSERT_SIZEOF(SetShaderGPRAllocation, 0x40);
    MARATHON_ASSERT_OFFSETOF(SetShaderGPRAllocation, m_Field0, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetShaderGPRAllocation, m_Field1, 0x3C);
}
