#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class GameMode : public SoX::Engine::DocMode
    {
    public:
        MARATHON_INSERT_PADDING(0x1C);
        xpointer<GameImp> m_pGameImp;
        MARATHON_INSERT_PADDING(0x1C);
    };
}
