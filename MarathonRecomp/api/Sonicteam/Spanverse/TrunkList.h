#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/AckAdminTag.h>

namespace Sonicteam::Spanverse
{
    struct TrunkList
    {
        xpointer<AckAdminTag> m_Prev;
        xpointer<AckAdminTag> m_Next;
        be<uint32_t> m_Size;
    };

    inline void AckAdminTag::FreeFromList(TrunkList* list)
    {
        AckAdminTag* m_pPrev; // r11
        AckAdminTag* m_pNext; // r11

        m_pPrev = this->m_pPrev;
        if (m_pPrev)
            m_pPrev->m_pNext = this->m_pNext;
        m_pNext = this->m_pNext;
        if (m_pNext)
            m_pNext->m_pPrev = this->m_pPrev;
        if (list->m_Prev == this)
            list->m_Prev = this->m_pNext;
        if (list->m_Next == this)
            list->m_Next = this->m_pPrev;

        list->m_Size = list->m_Size - 1;
        this->m_pNext = 0;
        this->m_pPrev = 0;
    }
}
