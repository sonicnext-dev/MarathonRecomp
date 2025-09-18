#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam
{
    class HUDPause : public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0xAC);
        be<float> m_TextPriority;
        MARATHON_INSERT_PADDING(0x44);
        bool m_ShowMissionWindow;
    };
}
