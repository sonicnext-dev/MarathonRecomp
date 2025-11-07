#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyGraphicsDevice.h>
#include <Sonicteam/SoX/Engine/RenderProcess.h>

namespace Sonicteam
{
    class MyRenderProcess : public SoX::Engine::RenderProcess
    {
    public:
        xpointer<RenderProcess> m_pParentProcess;
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
    };

    MARATHON_ASSERT_SIZEOF(MyRenderProcess, 0x38);
    MARATHON_ASSERT_OFFSETOF(MyRenderProcess, m_pParentProcess, 0x30);
    MARATHON_ASSERT_OFFSETOF(MyRenderProcess, m_pMyGraphicsDevice, 0x34);
}
