#pragma once

#include <Marathon.inl>
#include <Sonicteam/HudTextParts.h>

namespace Sonicteam
{
    class HUDOption : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x188);
        xpointer<HudTextParts> m_pHudTextRoot;
        MARATHON_INSERT_PADDING(8);
    };

    MARATHON_ASSERT_OFFSETOF(HUDOption, m_pHudTextRoot, 0x1DC);
    MARATHON_ASSERT_SIZEOF(HUDOption, 0x1E8);
}
