#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::State
{
    class ICommonContextIF
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(ICommonContextIF, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(ICommonContextIF, 4);
}
