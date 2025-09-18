#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam
{
    class HUDPause : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        xpointer<CsdObject> m_pCsdObject;
        MARATHON_INSERT_PADDING(0xA0);
        be<float> m_TextPriority;
        MARATHON_INSERT_PADDING(0x44);
        bool m_ShowMissionWindow;
    };
}
