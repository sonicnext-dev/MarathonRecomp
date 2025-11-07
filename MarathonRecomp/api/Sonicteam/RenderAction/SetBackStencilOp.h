#pragma once

#include <Marathon.inl>
#include <d3dxb.h>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetBackStencilOp : public MyRenderProcess
    {
    public:
        be<D3DXBSTENCILOP> m_StencilOp1;
        be<D3DXBSTENCILOP> m_StencilOp2;
        be<D3DXBSTENCILOP> m_StencilOp3;
    };

    MARATHON_ASSERT_SIZEOF(SetBackStencilOp, 0x44);
    MARATHON_ASSERT_OFFSETOF(SetBackStencilOp, m_StencilOp1, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetBackStencilOp, m_StencilOp2, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetBackStencilOp, m_StencilOp3, 0x40);
}
