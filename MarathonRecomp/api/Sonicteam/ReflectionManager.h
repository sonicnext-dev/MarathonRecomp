#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/Graphics/FrameGP.h>

namespace Sonicteam
{
    class ReflectionManager : public SoX::Engine::Task
    {
    public:
        xpointer<SoX::Graphics::FrameGP> m_pFrameGP;
    };

    MARATHON_ASSERT_OFFSETOF(ReflectionManager, m_pFrameGP, 0x4C);
    MARATHON_ASSERT_SIZEOF(ReflectionManager, 0x50);
}
