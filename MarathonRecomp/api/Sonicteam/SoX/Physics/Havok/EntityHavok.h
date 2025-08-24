#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Physics/Havok/WorldHavok.h>
#include <Physics/RigidBody.h>

namespace Sonicteam::SoX::Physics::Havok
{
    class EntityHavok
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0xC);
        xpointer<Phys::RigidBody> m_RigidBody;
        xpointer<SoX::Physics::Havok::WorldHavok> m_RefWorldHavok; //Ref_type
        Sonicteam::SoX::Math::Vector m_Rotation;
        Sonicteam::SoX::Math::Vector m_Translation;
        MARATHON_INSERT_PADDING(0xC);
    };
}
