#pragma once

#include <Marathon.inl>
#include <Sonicteam/Particles/ParticleRef.h>

namespace Sonicteam::Particles
{
    class Particle : public ParticleRef
    {
    public:
        struct Vftable : ParticleRef::Vftable
        {
            be<uint32_t> fpFunc08;
            be<uint32_t> fpFunc0C;
            be<uint32_t> fpFunc10; // Hint: used before Release() with arg 0.
            be<uint32_t> fpFunc14;
            be<uint32_t> fpFunc18;
            be<uint32_t> fpFunc1C;
            be<uint32_t> fpFunc20;
        };

        void Func10(uint32_t flags)
        {
            GuestToHostFunction<void>(((Vftable*)m_pVftable.get())->fpFunc10, this, flags);
        }

        void Func1C(Sonicteam::SoX::Graphics::Frame* Frame)
        {
            guest_heap_var<SoX::RefSharedPointer<SoX::Graphics::Frame>> frame = Frame;
            MARATHON_CALL_VIRTUAL_FUNCTION(ParticleRef, void, &Vftable::fpFunc1C, this, frame.get());
        }
    };
}
