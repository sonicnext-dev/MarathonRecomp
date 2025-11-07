#pragma once

#include <Marathon.inl>
#include <d3dxb.h>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetStencilOp : public MyRenderProcess
    {
    public:
        be<D3DXBSTENCILOP> m_FrontStencilOp;
        be<D3DXBSTENCILOP> m_BackStencilOp;
    };

    MARATHON_INSERT_PADDING(0x44);
    MARATHON_ASSERT_OFFSETOF(SetStencilOp, m_FrontStencilOp, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetStencilOp, m_BackStencilOp, 0x3C);
}
