#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/Havok/EntityHavokImp.h>
#include <Sonicteam/SoX/Physics/Phantom.h>

namespace Sonicteam::SoX::Physics::Havok
{
    class PhantomHavok : public EntityHavokImp<Phantom> {};
}
