#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX
{
    class RefCountObject
    {
    public:
        struct Vftable
        {
            be<uint32_t> Destroy;
        };

        xpointer<Vftable> m_pVftable;
        be<uint32_t> m_ReferenceCount;

        void Release(uint32_t Flag = 1)
        {
            m_ReferenceCount = m_ReferenceCount - 1;
            if (!m_ReferenceCount.get())
                Destroy(1);
        }

        inline void AddRef()
        {
            m_ReferenceCount = m_ReferenceCount + 1;
        }

        //Virtual
        void* Destroy(uint32_t flag)
        {
            return GuestToHostFunction<void*>(m_pVftable->Destroy,this,flag);
        }

    };
}
