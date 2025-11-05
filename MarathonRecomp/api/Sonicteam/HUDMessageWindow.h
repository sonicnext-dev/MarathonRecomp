#pragma once

#include <Marathon.inl>
#include <Sonicteam/HintWindowTask.h>

namespace Sonicteam
{
    class HUDMessageWindow : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        xpointer<CsdObject> m_pCsdObject;
        MARATHON_INSERT_PADDING(0x1C);
        xpointer<HUDMessageWindow> m_pParent;
        MARATHON_INSERT_PADDING(0x14);
        xpointer<HintWindowTask> m_pHintWindowTask;
    };

    MARATHON_ASSERT_OFFSETOF(HUDMessageWindow, m_pCsdObject, 0x54);
    MARATHON_ASSERT_OFFSETOF(HUDMessageWindow, m_pParent, 0x74);
    MARATHON_ASSERT_OFFSETOF(HUDMessageWindow, m_pHintWindowTask, 0x8C);
    MARATHON_ASSERT_SIZEOF(HUDMessageWindow, 0x90);
}
