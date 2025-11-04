#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyGraphicsDevice.h>
#include <Sonicteam/SoX/Engine/RenderProcess.h>

namespace Sonicteam
{
    class MyRenderProcess : public SoX::Engine::RenderProcess
    {
    public:
        xpointer<RenderProcess> m_pRenderProcess;
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
    };

    MARATHON_ASSERT_SIZEOF(MyRenderProcess, 0x38);
}
