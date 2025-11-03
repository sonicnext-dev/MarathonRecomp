#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IVariable
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(IVariable, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IVariable, 4);
}
