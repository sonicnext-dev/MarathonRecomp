#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Physics/ShapeCastListener.h>
#include <Sonicteam/SoX/Physics/World.h>
#include <hk330/hkpShape.h>

namespace Sonicteam::SoX::Physics
{
    struct RayCastRay
    {
        be<uint32_t> Flag;
        SoX::Math::Vector Field10;
        SoX::Math::Vector Field20;
    };

    class RayCast : public SoX::RefCountObject 
    {
    public:
        struct Vftable : RefCountObject::Vftable
        {
            be<uint32_t> fpFunc4;
            be<uint32_t> fpGetName;
            be<uint32_t> fpFuncC;
            be<uint32_t> fpFunc10;
            be<uint32_t> fpFunc14;
        };

        void Func4(RayCastRay* ray)
        {
            auto vft = (Vftable*)m_pVftable.get();
            GuestToHostFunction<void>(vft->fpFunc4, this, ray);
        }

        void Func4(RayCastRay& ray)
        {
            Func4(guest_heap_var<RayCastRay>(ray).get());
        }

        void Func8(SoX::RefSharedPointer<SoX::Physics::World>* world)
        {
            auto vft = (Vftable*)m_pVftable.get();
            world->get()->AddReference();
            GuestToHostFunction<void>(vft->fpGetName, this, world);
        }

        void Func8(SoX::RefSharedPointer<SoX::Physics::World>& ray)
        {
            Func8(guest_heap_var<SoX::RefSharedPointer<SoX::Physics::World>>(ray).get());
        }

        void Func14(SoX::Math::Vector* points, SoX::Physics::ShapeCastListener* listener)
        {
            auto vft = (Vftable*)m_pVftable.get();
            GuestToHostFunction<void>(vft->fpFunc14,this, points, listener);
        }
    };
}
