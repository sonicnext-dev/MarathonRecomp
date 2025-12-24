#pragma once

#include <Marathon.inl>

namespace Sonicteam::Spanverse
{
    struct TrunkList;

    class AckAdminTag
    {
    public:
        xpointer<void> m_pVftable;
        xpointer<AckAdminTag> m_pPrev;
        xpointer<AckAdminTag> m_pNext;

        void FreeFromList(TrunkList* list);
    };

    MARATHON_ASSERT_SIZEOF(AckAdminTag, 0xC);
}
