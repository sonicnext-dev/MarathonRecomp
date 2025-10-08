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
    class EntityRef;

    class Entity : public MessageReceiver, public RefCountObject
    {
    public:
        struct Vftable : public MessageReceiver::Vftable
        {
            MARATHON_INSERT_PADDING(0x30);
            be<uint32_t> fpIsInitializedToWorld;
            MARATHON_INSERT_PADDING(0x4);
            be<uint32_t> InitializeToWorld;
            MARATHON_INSERT_PADDING(0x18);
            be<uint32_t> SetPhantomListener;
            MARATHON_INSERT_PADDING(0x30);
            be<uint32_t> fpApplyForce1; //    void (__cdecl *applyForce)(hkpMotion *this, const float, const hkVector4 *, const hkVector4 *); linear
            MARATHON_INSERT_PADDING(0xC);
            be<uint32_t> fpApplyForce2; //    void (__cdecl *applyForce)(hkpMotion *this, const float, const hkVector4 *); angular
            be<uint32_t> fpGetLinearVelocity;
            be<uint32_t> fpSetLinearVelocity;
            be<uint32_t> fpGetAngularVelocity;
            be<uint32_t> fpSetAngularVelocity;
        };

        MARATHON_INSERT_PADDING(0x4);
        SoX::LinkNode<EntityRef> m_lnEntity;
        RefSharedPointer<Shape> m_spPhantomListener;
        xpointer<MessageReceiver> m_pReceiver;
        RefSharedPointer<Shape> m_spShape;

        bool IsInitializedToWorld()
        {
            auto vft = static_cast<Vftable*>(MessageReceiver::m_pVftable.get());
            return GuestToHostFunction<bool>(vft->fpIsInitializedToWorld, this);
        }

        void InitializeToWorld(RefSharedPointer<World>& world)
        {
            auto vft = static_cast<Vftable*>(MessageReceiver::m_pVftable.get());
            GuestToHostFunction<void>(vft->InitializeToWorld, this, &world);
        }

        void SetPhantomListener(RefSharedPointer<PhantomListener>& phantomListener)
        {
            auto vft = static_cast<Vftable*>(MessageReceiver::m_pVftable.get());
            GuestToHostFunction<void>(vft->SetPhantomListener, this, guest_heap_var<RefSharedPointer<PhantomListener>>(phantomListener).get());
        }

        SoX::Math::Vector GetAngularVelocity()
        {
            guest_heap_var<SoX::Math::Vector> ret;
            auto vft = static_cast<Vftable*>(MessageReceiver::m_pVftable.get());
            GuestToHostFunction<void>(vft->fpGetAngularVelocity,ret.get(), this);
            return *ret.get();
        }

        SoX::Math::Vector GetLinearVelocity()
        {
            guest_heap_var<SoX::Math::Vector> ret;
            auto vft = static_cast<Vftable*>(MessageReceiver::m_pVftable.get());
            GuestToHostFunction<void>(vft->fpGetLinearVelocity,ret.get(), this);
            return *ret.get();
        }
    };

    MARATHON_ASSERT_SIZEOF(Entity, 0x28);
    MARATHON_ASSERT_OFFSETOF(Entity::Vftable, fpApplyForce1, 0x90);
    MARATHON_ASSERT_OFFSETOF(Entity::Vftable, fpApplyForce2, 0xA0);
    MARATHON_ASSERT_OFFSETOF(Entity::Vftable, fpGetLinearVelocity, 0xA4);
}
