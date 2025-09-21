#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class PauseTask : public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x230);
        xpointer<TextEntity> m_pMissionText;
    };
}
