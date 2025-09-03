#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/MessageReceiver.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Physics/Shape.h>
#include <Sonicteam/SoX/Physics/World.h>
#include <Sonicteam/SoX/Physics/PhantomListener.h>

namespace Sonicteam::SoX::Physics
{
    class Entity : public MessageReceiver, public RefCountObject
    {
    public:
        struct Vftable : public MessageReceiver::Vftable
        {
            MARATHON_INSERT_PADDING(0x38);
            be<uint32_t> InitializeToWorld;
            MARATHON_INSERT_PADDING(0x18);
            be<uint32_t> SetPhantomListener;
        };

        MARATHON_INSERT_PADDING(0x10);
        RefSharedPointer<Shape> m_spPhantomListener;
        xpointer<MessageReceiver> m_pReceiver;
        RefSharedPointer<Shape> m_spShape;

        void InitializeToWorld(RefSharedPointer<World>& world)
        {
            auto vft = static_cast<Vftable*>(MessageReceiver::m_pVftable.get());
            GuestToHostFunction<void>(vft->InitializeToWorld, this, &world);
        }

        void SetPhantomListener(RefSharedPointer<PhantomListener>& phantomListener)
        {
            auto vft = static_cast<Vftable*>(MessageReceiver::m_pVftable.get());
            GuestToHostFunction<void>(vft->SetPhantomListener, this, &phantomListener);
        }
    };
}
