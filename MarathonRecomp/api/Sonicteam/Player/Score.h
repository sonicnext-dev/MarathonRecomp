#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class Score : public IScore, public IVariable, public IStepable
    {
    public:
        MARATHON_INSERT_PADDING(0x0C);
        xpointer<Sonicteam::Player::Object> m_pPlayer;
        MARATHON_INSERT_PADDING(0x38);
    };
    
    MARATHON_ASSERT_OFFSETOF(Score, m_pPlayer, 0x34);
    MARATHON_ASSERT_SIZEOF(Score, 0x70);
}
