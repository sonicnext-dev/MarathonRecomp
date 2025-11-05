#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/World.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/Player/RootFrame.h>

namespace Sonicteam::Player
{
    template <typename TCollisionListener>
    class IPostureSupportRayTemplate
    {
    public:
        xpointer<void> m_pVftable;
        SoX::RefSharedPointer<SoX::Physics::World> m_spWorld;
        boost::shared_ptr<TCollisionListener> m_spCollisionListener;
        xpointer<Player::RootFrame> m_pRootFrame;
    };

    MARATHON_ASSERT_OFFSETOF(IPostureSupportRayTemplate<void>, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportRayTemplate<void>, m_spWorld, 0x04);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportRayTemplate<void>, m_spCollisionListener, 0x08);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportRayTemplate<void>, m_pRootFrame, 0x10);
    MARATHON_ASSERT_SIZEOF(IPostureSupportRayTemplate<void>, 0x14);
}
