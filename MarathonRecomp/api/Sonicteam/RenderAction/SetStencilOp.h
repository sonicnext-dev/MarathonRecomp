#pragma once

#include <Marathon.inl>
#include <d3dxb.h>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetStencilOp : public MyRenderProcess
    {
    public:
        be<D3DXBSTENCILOP> m_Field38;
        be<D3DXBSTENCILOP> m_Field3C;
        be<D3DXBSTENCILOP> m_Field40;
    };

    MARATHON_ASSERT_OFFSETOF(SetStencilOp, m_Field38, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetStencilOp, m_Field3C, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetStencilOp, m_Field40, 0x40);
    MARATHON_ASSERT_SIZEOF(SetStencilOp, 0x44);
}
