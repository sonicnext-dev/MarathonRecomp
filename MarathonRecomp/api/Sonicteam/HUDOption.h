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
    };
}
