#pragma once

#include <Marathon.inl>
#include <d3dxb.h>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetStencilFunc : public MyRenderProcess
    {
    public:
        be<D3DXBSTENCILOP> m_StencilMode;
        be<uint32_t> m_FrontReference;
        be<uint32_t> m_BackReference;
    };

    MARATHON_ASSERT_SIZEOF(SetStencilFunc, 0x44);
    MARATHON_ASSERT_OFFSETOF(SetStencilFunc, m_StencilMode, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetStencilFunc, m_FrontReference, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetStencilFunc, m_BackReference, 0x40);
}
