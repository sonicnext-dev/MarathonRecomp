#pragma once

namespace Sonicteam::Player
{
    inline void ICollisionListener::SetPostureRequestFlag(uint32_t postureRequestFlag)
    {
        GuestToHostFunction<void>(m_pVftable->fpSetPostureRequestFlag, this, postureRequestFlag);
    }

    inline void ICollisionListener::Update()
    {
        GuestToHostFunction<void>(m_pVftable->fpUpdate, this);
    }

    inline SoX::Math::Vector ICollisionListener::GetContactPosition()
    {
        guest_heap_var<SoX::Math::Vector> ret;
        GuestToHostFunction<SoX::Math::Vector*>(m_pVftable->fpGetContactPosition, ret.get(), this);
        return *ret;
    }

    inline SoX::Math::Vector ICollisionListener::GetContactNormal()
    {
        guest_heap_var<SoX::Math::Vector> result;
        GuestToHostFunction<void>(m_pVftable->fpGetContactNormal, result.get(), this);
        return *result;
    }

    inline float ICollisionListener::GetLength(float normalize = 0.0)
    {
        return GuestToHostFunction<float>(m_pVftable->fpGetLength, this, normalize);
    }

    inline uint32_t ICollisionListener::GetCollisionFlag()
    {
        return GuestToHostFunction<uint32_t>(m_pVftable->fpGetCollisionFlag, this);
    }

    inline SoX::Physics::EntityRefWrapper ICollisionListener::GetContactEntity()
    {
        guest_heap_var<SoX::Physics::EntityRef,false,false> ret;
        GuestToHostFunction<SoX::Physics::EntityRef*>(m_pVftable->GetContactEntity, ret.get(), this);
        return (SoX::Physics::EntityRefWrapper)ret.get();
    }

    inline uint32_t ICollisionListener::GetCurrentSurfaceContacts()
    {
        return GuestToHostFunction<uint32_t>(m_pVftable->fpGetCurrentSurfaceContacts, this);
    }

    inline uint32_t ICollisionListener::GetRayFlag()
    {
        return GuestToHostFunction<uint32_t>(m_pVftable->fpGetRayFlag, this);
    }
}
