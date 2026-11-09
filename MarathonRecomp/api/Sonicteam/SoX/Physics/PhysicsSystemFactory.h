#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/Phantom.h>

namespace Sonicteam::SoX::Physics
{
    class PhysicsSystemFactory
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpCreateSphereShape;
            be<uint32_t> fpCreateBoxShape;
            be<uint32_t> fpCreateCapsuleShape;
            be<uint32_t> fpCreateCylinderShape;
            be<uint32_t> fpCreateMeshShape;
            be<uint32_t> fpCreateConvexShape;
            be<uint32_t> fpCreateBodyA;
            be<uint32_t> fpCreateBodyB;
            be<uint32_t> fpCreatePhantomA;
            be<uint32_t> fpCreatePhantomB;
            be<uint32_t> fpCreateRayCastA;
            be<uint32_t> fpCreateRayCastB;
            be<uint32_t> fpCreateShapeCastA;
            be<uint32_t> fpCreateShapeCastB;
            be<uint32_t> fpCreateLineConstraint;
            be<uint32_t> fpCreateRagdollInfo;
            be<uint32_t> fpCreateVehicleInfo;
            be<uint32_t> fpCreateLandCollisionInfo;
        };

        xpointer<Vftable> m_pVftable;

        Phantom* CreatePhantomB()
        {
            return GuestToHostFunction<Phantom*>(m_pVftable->fpCreatePhantomB.get(), this);
        }
    };

    MARATHON_ASSERT_SIZEOF(PhysicsSystemFactory, 0x04);
}
