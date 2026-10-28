#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class PicBase
    {
    public:
        xpointer<void> m_pVftable;
        be<uint32_t> m_State1;
        be<uint32_t> m_State2;
        be<uint32_t> m_State3;
    };

    MARATHON_ASSERT_OFFSETOF(PicBase, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(PicBase, m_State1, 0x04);
    MARATHON_ASSERT_OFFSETOF(PicBase, m_State2, 0x08);
    MARATHON_ASSERT_OFFSETOF(PicBase, m_State3, 0x0C);
    MARATHON_ASSERT_SIZEOF(PicBase, 0x10);
}
