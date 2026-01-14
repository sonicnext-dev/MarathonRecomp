#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/ShapeCast.h>
#include <hk330/hkpPhantom.h>

namespace Sonicteam::SoX::Physics::Havok
{
    class RayCastHavok : public Physics::RayCast 
    {
    public:
        xpointer<hk330::hkpPhantom> m_HavokRayCast; //not sure about base class
    };

    MARATHON_ASSERT_SIZEOF(RayCastHavok, 0xC);
}
