#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class HUDMainMenu : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x4FC);
        xpointer<HudTextParts> m_pHudTextRoot;
    };
}
