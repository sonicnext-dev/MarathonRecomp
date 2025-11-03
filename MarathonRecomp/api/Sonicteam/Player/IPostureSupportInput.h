#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IPostureSupportInput
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x2C);
    };

    MARATHON_ASSERT_OFFSETOF(IPostureSupportInput, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IPostureSupportInput, 0x30);
}
