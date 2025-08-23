#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class CsdLink
    {
    public:
        xpointer<void> m_pVftable;
        xpointer<CsdLink> m_pNext;
        xpointer<CsdLink> m_pPrevious;
        be<float> m_Priority;
        MARATHON_INSERT_PADDING(8);
    };
}
