#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IStepable
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(IStepable, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IStepable, 4);
}
