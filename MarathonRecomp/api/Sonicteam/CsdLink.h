#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class CsdLink
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            MARATHON_INSERT_PADDING(4);
            be<uint32_t> fpUpdate;
        };

        xpointer<Vftable> m_pVftable;
        xpointer<CsdLink> m_pNext;
        xpointer<CsdLink> m_pPrevious;
        be<float> m_Priority;
        MARATHON_INSERT_PADDING(8);

        void* Destroy(uint32_t flags = 1)
        {
            return GuestToHostFunction<void*>(m_pVftable->fpDestroy.get(), this, flags);
        }

        int Update(double deltaTime = 0.0)
        {
            return GuestToHostFunction<int>(m_pVftable->fpUpdate.get(), this, deltaTime);
        }
    };
}
