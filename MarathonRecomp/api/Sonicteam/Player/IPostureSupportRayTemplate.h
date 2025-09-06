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
}
