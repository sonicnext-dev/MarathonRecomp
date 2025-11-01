#pragma once

#include <Marathon.inl>
#include <Sonicteam/Particles/ParticleRef.h>

namespace Sonicteam::Particles
{
    class Particle : public ParticleRef 
    {
        struct Vftable : ParticleRef::Vftable
        {
            be<uint32_t> fpFunc8;
            be<uint32_t> fpFuncC;
            be<uint32_t> fpFunc10; // Hint : Used Before Release() with arg 0
            be<uint32_t> fpFunc14;
            be<uint32_t> fpFunc18;
            be<uint32_t> fpFunc1C;
            be<uint32_t> fpFunc20;
        };

       void Func10(uint32_t flag)
       {
           auto pVft = (Vftable*)(m_pVftable.get());
           GuestToHostFunction<void>(pVft->fpFunc10, this, flag);
       }

    };
}
