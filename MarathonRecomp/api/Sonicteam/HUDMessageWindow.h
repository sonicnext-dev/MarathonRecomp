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
        xpointer<HUDMessageWindow> m_pThis;
        MARATHON_INSERT_PADDING(0x14);
        xpointer<HintWindowTask> m_pHintWindowTask;
    };
}
