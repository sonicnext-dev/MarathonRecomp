#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/CsdObject.h>
#include <Sonicteam/HudTextParts.h>

namespace Sonicteam
{
    class HUDStageTitle : public SoX::Engine::Task
    {
    public:
        xpointer<CsdObject> m_pCsdTitle;
        xpointer<CsdObject> m_pCsdOverlay;
        xpointer<HudTextParts> m_pHudTextParts;
    };

    MARATHON_ASSERT_OFFSETOF(HUDStageTitle, m_pCsdTitle, 0x4C);
    MARATHON_ASSERT_OFFSETOF(HUDStageTitle, m_pCsdOverlay, 0x50);
    MARATHON_ASSERT_OFFSETOF(HUDStageTitle, m_pHudTextParts, 0x54);
    MARATHON_ASSERT_SIZEOF(HUDStageTitle, 0x58);
}
