#pragma once

#include <Marathon.inl>
#include <Sonicteam/GameImp.h>

namespace Sonicteam
{
    class GameMode : public SoX::Engine::DocMode
    {
    public:
        MARATHON_INSERT_PADDING(0x1C);
        xpointer<Game> m_pGame;
        MARATHON_INSERT_PADDING(0x1C);

        GameImp* GetGame() const
        {
            return (GameImp*)m_pGame.get();
        }
    };

    MARATHON_ASSERT_OFFSETOF(GameMode, m_pGame, 0x6C);
    MARATHON_ASSERT_SIZEOF(GameMode, 0x8C);
}
