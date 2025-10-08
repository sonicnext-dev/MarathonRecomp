#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/World.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

namespace Sonicteam::Player
{
    template <typename CollisionListener>
    class IPostureSupportRayTemplate
    {
    public:
        xpointer<void> m_pVftable;
        SoX::RefSharedPointer<SoX::Physics::World> m_spWorld;
        boost::shared_ptr<CollisionListener> m_spCollisionListener;
        xpointer<be<uint32_t>> m_pPostureRequestFlag;
    };

    MARATHON_ASSERT_SIZEOF(IPostureSupportRayTemplate<uint32_t>, 20);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportRayTemplate<uint32_t>, m_spWorld, 0x4);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportRayTemplate<uint32_t>, m_spCollisionListener, 0x8);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportRayTemplate<uint32_t>, m_pPostureRequestFlag, 0x10);
}
