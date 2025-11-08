#pragma once

#include <Marathon.inl>

namespace Sonicteam::Particles
{
    class ParticleManagerUnit
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpFunc04; // RefSharedPointer<Sonicteam::Particles::Particle> LoadBank(return, this, ?, ?)
            be<uint32_t> fpFunc08;
            be<uint32_t> fpFunc0C;
            be<uint32_t> fpFunc10;
            be<uint32_t> fpFunc14;
            be<uint32_t> fpFunc18;
            be<uint32_t> fpFunc1C;
            be<uint32_t> fpFunc20;
            be<uint32_t> fpFunc24;
            be<uint32_t> fpFunc2C;
        };

        xpointer<Vftable> m_pVftable;
    };
}
