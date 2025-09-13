#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/HudTextParts.h>

namespace Sonicteam
{
    class HUDButtonWindow : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x38);
        xpointer<HudTextParts> m_pHudTextParts;
    };
}
