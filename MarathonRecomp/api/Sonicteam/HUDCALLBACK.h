#pragma once

namespace Sonicteam
{
    class HUDCALLBACK
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(HUDCALLBACK, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(HUDCALLBACK, 4);
}
