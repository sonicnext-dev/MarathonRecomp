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
}
