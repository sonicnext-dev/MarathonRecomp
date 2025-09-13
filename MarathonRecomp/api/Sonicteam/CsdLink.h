#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class CsdLink
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(8);
            be<uint32_t> fpUpdate;
        };

        xpointer<Vftable> m_pVftable;
        xpointer<CsdLink> m_pNext;
        xpointer<CsdLink> m_pPrevious;
        be<float> m_Priority;
        MARATHON_INSERT_PADDING(8);

        int Update(double deltaTime = 0.0)
        {
            return GuestToHostFunction<int>(m_pVftable->fpUpdate.get(), this, deltaTime);
        }
    };
}
