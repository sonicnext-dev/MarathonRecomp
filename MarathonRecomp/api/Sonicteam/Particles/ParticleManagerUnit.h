#pragma once

#include <Marathon.inl>
#include <Sonicteam/Particles/ParticleRef.h>
#include <Sonicteam/DocMarathonImp.h>

namespace Sonicteam::Particles
{
    class ParticleManagerUnit
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpFunc4; // RefSharedPointer<Sonicteam::Particles::Particle> LoadBank(return,this,?,?)
            be<uint32_t> fpFunc8;
            be<uint32_t> fpFunc0xC;
            be<uint32_t> fpFunc0x10;
            be<uint32_t> fpFunc0x14;
            be<uint32_t> fpFunc0x18;
            be<uint32_t> fpFunc0x1C;
            be<uint32_t> fpFunc0x20;
            be<uint32_t> fpFunc0x24;
            be<uint32_t> fpFunc0x2C;
        };

        xpointer<Vftable> m_pVftable;
    };
}
