#pragma once

#include <Sonicteam/GameImp.h>

namespace Sonicteam
{
    class PauseAdapter : public SoX::Engine::Task
    {
    public:
        xpointer<GameImp> m_pGameImp;
        be<uint32_t> m_SelectedID;
        be<uint32_t> m_Field54;
    };
}
