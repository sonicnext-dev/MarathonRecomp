#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Physics/Havok/WorldHavok.h>
#include <hk330/hkpRigidBody.h>

namespace Sonicteam::SoX::Physics::Havok
{
    class EntityHavok
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0xC);
        xpointer<hk330::hkpRigidBody> m_pRigidBody;
        RefSharedPointer<SoX::Physics::Havok::WorldHavok> m_spWorldHavok;
        Sonicteam::SoX::Math::Vector m_Rotation;
        Sonicteam::SoX::Math::Vector m_Translation;
        MARATHON_INSERT_PADDING(0xC);
    };
}
