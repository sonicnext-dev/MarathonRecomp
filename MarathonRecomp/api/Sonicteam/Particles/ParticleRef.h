#pragma once

#include <Marathon.inl>

namespace Sonicteam::Particles
{
    class ParticleRef
    {
    public:
        
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpRelease;
        };

        void Destroy(uint32_t flag)
        {
            GuestToHostFunction<void>(m_pVftable->fpDestroy, this, flag);
        }

        void Release()
        {
            GuestToHostFunction<void>(m_pVftable->fpRelease, this);
        }

        xpointer<Vftable> m_pVftable;
        be<uint32_t> m_RefCount;
    };
}
