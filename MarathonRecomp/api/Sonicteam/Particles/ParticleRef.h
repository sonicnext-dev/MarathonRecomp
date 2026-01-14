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

        xpointer<Vftable> m_pVftable;
        be<uint32_t> m_ReferenceCount;

        void Destroy(uint8_t flags = 1)
        {
            GuestToHostFunction<void>(m_pVftable->fpDestroy, this, flags);
        }

        void Release()
        {
            GuestToHostFunction<void>(m_pVftable->fpRelease, this);
        }
    };
}
