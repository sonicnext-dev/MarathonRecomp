#pragma once

#include <Marathon.inl>
#include <d3dxb.h>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetBackStencilOp : public MyRenderProcess
    {
    public:
        be<D3DXBSTENCILOP> m_Field38;
        be<D3DXBSTENCILOP> m_Field3C;
        be<D3DXBSTENCILOP> m_Field40;
    };

    MARATHON_ASSERT_OFFSETOF(SetBackStencilOp, m_Field38, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetBackStencilOp, m_Field3C, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetBackStencilOp, m_Field40, 0x40);
    MARATHON_ASSERT_SIZEOF(SetBackStencilOp, 0x44);
}
