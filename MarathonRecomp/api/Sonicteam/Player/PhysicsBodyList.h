#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/SoX/Physics/EntityRef.h>
#include <api/stdx/vector.h>

namespace Sonicteam::Player
{
    class PhysicsBodyList : public IPlugIn
    {
    public:
        stdx::vector<SoX::Physics::EntityRef> m_vlrBody;
        be<uint32_t> m_ActorID; // Target

        bool sub_8223B148(SoX::Physics::EntityRef* entityRef);
    };

    MARATHON_ASSERT_SIZEOF(PhysicsBodyList, 0x34);
    MARATHON_ASSERT_OFFSETOF(PhysicsBodyList, m_vlrBody, 0x20);
    MARATHON_ASSERT_OFFSETOF(PhysicsBodyList, m_ActorID, 0x30);
}

#include "PhysicsBodyList.inl"
