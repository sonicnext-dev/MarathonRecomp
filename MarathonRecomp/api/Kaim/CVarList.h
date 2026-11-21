#pragma once

#include <Marathon.inl>
#include <Kaim/CObject.h>

namespace Kaim
{
    template <typename T>
    class CVarList : public CObject
    {
    public:
        class CCell : public CObject
        {
        public:
            T m_data;
            xpointer<CCell> m_previous;
            xpointer<CCell> m_next;
            bool m_inUse;
        };

        class CIterator : public CObject
        {
        public:
            xpointer<CCell> m_cell;
        };

        xpointer<CCell> m_cells;
        xpointer<CCell> m_first;
        xpointer<CCell> m_last;
        xpointer<CCell> m_firstFree;
        be<uint32_t> m_count;
        const be<uint32_t> m_maxSize;
    };

    MARATHON_ASSERT_OFFSETOF(CVarList<void>::CIterator, m_cell, 0x04);
    MARATHON_ASSERT_SIZEOF(CVarList<void>::CIterator, 0x08);

    MARATHON_ASSERT_OFFSETOF(CVarList<void>, m_cells, 0x04);
    MARATHON_ASSERT_OFFSETOF(CVarList<void>, m_first, 0x08);
    MARATHON_ASSERT_OFFSETOF(CVarList<void>, m_last, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CVarList<void>, m_firstFree, 0x10);
    MARATHON_ASSERT_OFFSETOF(CVarList<void>, m_count, 0x14);
    MARATHON_ASSERT_OFFSETOF(CVarList<void>, m_maxSize, 0x18);
    MARATHON_ASSERT_SIZEOF(CVarList<void>, 0x1C);
}
