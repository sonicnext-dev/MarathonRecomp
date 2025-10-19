#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX
{
    class RefCountObject
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
        };

        xpointer<Vftable> m_pVftable;
        be<uint32_t> m_ReferenceCount;

        void* Destroy(uint32_t flag)
        {
            return GuestToHostFunction<void*>(m_pVftable->fpDestroy, this, flag);
        }

        void Release(uint32_t flag = 1)
        {
            m_ReferenceCount = m_ReferenceCount - 1;

            if (!m_ReferenceCount.get())
                Destroy(flag);
        }

        inline void AddReference()
        {
            m_ReferenceCount = m_ReferenceCount + 1;
        }
    };
}
