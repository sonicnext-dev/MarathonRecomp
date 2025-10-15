#pragma once

#include <Sonicteam/GameImp.h>

namespace Sonicteam
{
    class PauseAdapter : public SoX::Engine::Task
    {
    public:
        xpointer<Game> m_pGame;
        be<uint32_t> m_SelectedID;
        be<uint32_t> m_Field54;

        GameImp* GetGame() const
        {
            return (GameImp*)m_pGame.get();
        }
    };
}
