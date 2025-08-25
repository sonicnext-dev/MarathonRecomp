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
    class Entity : public SoX::MessageReceiver, public SoX::RefCountObject
    {
    public:
        struct Vftable:MessageReceiver::Vftable
        {
            MARATHON_INSERT_PADDING(0x38);
            be<uint32_t> InitializeToWorld;
            MARATHON_INSERT_PADDING(0x18);
            be<uint32_t> SetPhantomListener;
        };

        MARATHON_INSERT_PADDING(0x10);
        SoX::RefSharedPointer<SoX::Physics::Shape> m_spPhantomListener;
        xpointer<SoX::MessageReceiver> m_pReceiver;
        SoX::RefSharedPointer<SoX::Physics::Shape> m_spShape;

        //Virtual
        void InitializeToWorld(SoX::RefSharedPointer<Sonicteam::SoX::Physics::World>& world)
        {
            auto vft = static_cast<Vftable*>(MessageReceiver::m_pVftable.get());
            GuestToHostFunction<void>(vft->InitializeToWorld, this, &world);
        }

        void SetPhantomListener(SoX::RefSharedPointer<PhantomListener>& phantomListener)
        {
            auto vft = static_cast<Vftable*>(MessageReceiver::m_pVftable.get());
            GuestToHostFunction<void>(vft->SetPhantomListener, this, &phantomListener);
        }
    };
}
