#pragma once

#include <Marathon.inl>

class CCalBase
{
public:
    xpointer<void> m_pVftable;
    xpointer<CCalBase> m_pPrev;
    xpointer<CCalBase> m_pNext;
    xpointer<uint32_t> m_ReferenceCount;
    MARATHON_INSERT_PADDING(0x1C);
};

MARATHON_ASSERT_OFFSETOF(CCalBase, m_pVftable, 0x00);
MARATHON_ASSERT_OFFSETOF(CCalBase, m_pPrev, 0x04);
MARATHON_ASSERT_OFFSETOF(CCalBase, m_pNext, 0x08);
MARATHON_ASSERT_OFFSETOF(CCalBase, m_ReferenceCount, 0x0C);
MARATHON_ASSERT_SIZEOF(CCalBase, 0x2C);
