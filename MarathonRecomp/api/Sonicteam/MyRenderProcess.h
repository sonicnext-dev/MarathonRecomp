#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/RenderProcess.h>

namespace Sonicteam
{
    class MyGraphicsDevice;

    class MyRenderProcess : public SoX::Engine::RenderProcess
    {
    public:
        xpointer<SoX::Engine::Doc> m_pDoc;
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
    };

    MARATHON_ASSERT_OFFSETOF(MyRenderProcess, m_pDoc, 0x30);
    MARATHON_ASSERT_OFFSETOF(MyRenderProcess, m_pMyGraphicsDevice, 0x34);
}
