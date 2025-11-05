#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class HUDStageTitle : public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(8);
        xpointer<HudTextParts> m_pHudTextParts;
    };

    MARATHON_ASSERT_OFFSETOF(HUDStageTitle, m_pHudTextParts, 0x54);
    MARATHON_ASSERT_SIZEOF(HUDStageTitle, 0x58);
}
