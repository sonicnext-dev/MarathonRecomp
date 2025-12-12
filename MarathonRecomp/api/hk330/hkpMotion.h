#pragma once

#include <Marathon.inl>
#include <hk330/hkReferencedObject.h>
#include <Sonicteam/SoX/Math/Vector.h>
using Vector = Sonicteam::SoX::Math::Vector;

namespace hk330
{
    class hkpMotion : public hkReferencedObject
    {
    public:

        struct Vftable : hkReferencedObject::Vftable
        {
            MARATHON_INSERT_PADDING(0x70);
            be<uint32_t> fpApplyForce;
        };

        MARATHON_INSERT_PADDING(0xB8);
        Sonicteam::SoX::Math::Vector m_inertiaAndMassInv; // Since m_linearVelocity is in the right place, "m_inertiaAndMassInv" should be here
        Sonicteam::SoX::Math::Vector m_linearVelocity;

        double GetMassInv()
        {
            if (m_inertiaAndMassInv.X != 0.0)
                return 1.0 / m_inertiaAndMassInv.X;
            return 0.0;
        }

        void ApplyForce(Vector* vempty, Vector* vforce, double delta)
        {
            auto vft = (Vftable*)m_pVftable.get();
            GuestToHostFunction<void>(vft->fpApplyForce, this, vempty, vforce, delta);
        }

        void ApplyForce(Vector vempty, Vector& vforce, double delta)
        {
            ApplyForce(guest_heap_var<Vector>(vempty).get(),
                guest_heap_var<Vector>(vforce).get(), delta);
        }

    };

    MARATHON_ASSERT_OFFSETOF(hkpMotion, m_inertiaAndMassInv, 0xC0);
    MARATHON_ASSERT_OFFSETOF(hkpMotion, m_linearVelocity, 0xD0);
    MARATHON_ASSERT_OFFSETOF(hkpMotion::Vftable, fpApplyForce, 0x74);
}
