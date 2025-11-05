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

    MARATHON_ASSERT_OFFSETOF(CsdLink, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(CsdLink, m_pNext, 0x04);
    MARATHON_ASSERT_OFFSETOF(CsdLink, m_pPrevious, 0x08);
    MARATHON_ASSERT_OFFSETOF(CsdLink, m_Priority, 0x0C);
    MARATHON_ASSERT_SIZEOF(CsdLink, 0x18);
}
