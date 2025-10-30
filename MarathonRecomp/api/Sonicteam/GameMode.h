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
}
