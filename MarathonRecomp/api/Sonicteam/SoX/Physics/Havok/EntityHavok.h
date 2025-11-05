#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Math/Quaternion.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Physics/Havok/WorldHavok.h>
#include <hk330/hkpRigidBody.h>

namespace Sonicteam::SoX::Physics::Havok
{
    class EntityHavok
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x0C);
        xpointer<hk330::hkpRigidBody> m_pRigidBody;
        RefSharedPointer<WorldHavok> m_spWorldHavok;
        Math::Quaternion m_Rotation;
        Math::Vector m_Translation;
        MARATHON_INSERT_PADDING(0x0C);
    };

    MARATHON_ASSERT_OFFSETOF(EntityHavok, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(EntityHavok, m_pRigidBody, 0x10);
    MARATHON_ASSERT_OFFSETOF(EntityHavok, m_spWorldHavok, 0x14);
    MARATHON_ASSERT_OFFSETOF(EntityHavok, m_Rotation, 0x20);
    MARATHON_ASSERT_OFFSETOF(EntityHavok, m_Translation, 0x30);
}
