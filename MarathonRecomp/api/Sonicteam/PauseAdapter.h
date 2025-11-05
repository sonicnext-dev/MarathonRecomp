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
        MARATHON_INSERT_PADDING(0x20);

        GameImp* GetGame() const
        {
            return (GameImp*)m_pGame.get();
        }
    };

    MARATHON_ASSERT_OFFSETOF(PauseAdapter, m_pGame, 0x4C);
    MARATHON_ASSERT_OFFSETOF(PauseAdapter, m_SelectedID, 0x50);
    MARATHON_ASSERT_OFFSETOF(PauseAdapter, m_Field54, 0x54);
    MARATHON_ASSERT_SIZEOF(PauseAdapter, 0x78);
}
