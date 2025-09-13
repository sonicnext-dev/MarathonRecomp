#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class MainDisplayTask : public SoX::Engine::Task
    {
    public:
        xpointer<HUDMainDisplay> m_pHUDMainDisplay;
    };
}
