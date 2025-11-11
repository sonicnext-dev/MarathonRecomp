#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam
{
    class MainMenuExpositionTask : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        be<uint32_t> m_TextMotionState;
    };

    MARATHON_ASSERT_OFFSETOF(MainMenuExpositionTask, m_TextMotionState, 0x54);
}
