#pragma once

namespace Sonicteam::Player
{
    class IEventerListener
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(8);
    };

    MARATHON_ASSERT_OFFSETOF(IEventerListener, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IEventerListener, 0x0C);
}
