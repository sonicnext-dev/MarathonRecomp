#pragma once

namespace Sonicteam::Player
{
    inline bool IPosturePlugIn::SetControllerMovement(SoX::RefSharedPointer<SoX::RefCountObject>* RefObj, SoX::Math::Quaternion* v)
    {
        return GuestToHostFunction<bool>(m_pVftable->fpSetControllerMovement, this, RefObj, v);
    }

    inline bool IPosturePlugIn::SetControllerMovement(SoX::RefSharedPointer<SoX::RefCountObject>& RefObj, SoX::Math::Quaternion& v)
    {
        guest_heap_var<SoX::Math::Quaternion> gsvV(v);
        guest_heap_var<SoX::RefSharedPointer<SoX::RefCountObject>> gsvObj(RefObj);
        bool result = SetControllerMovement(gsvObj.get(), gsvV.get());
        return result;
    }

    inline bool IPosturePlugIn::IsInContact()
    {
        return GuestToHostFunction<bool>(m_pVftable->fpIsInContact, this);
    }

    inline bool IPosturePlugIn::IsOffContact()
    {
        return GuestToHostFunction<bool>(m_pVftable->fpIsOffContact, this);
    }

    inline uint32_t IPosturePlugIn::GetContactFlag()
    {
        return GuestToHostFunction<uint32_t>(m_pVftable->fpGetContactFlag, this);
    }

    inline void IPosturePlugIn::OutContactPosition(SoX::Math::Vector* vector)
    {
        GuestToHostFunction<void>(m_pVftable->fpOutContactPosition, this, vector);
    }

    inline void IPosturePlugIn::OutContactUP(SoX::Math::Vector* quat)
    {
        GuestToHostFunction<void>(m_pVftable->fpOutContactUP, this, quat);
    }

    inline uint32_t IPosturePlugIn::Func20(SoX::Math::Vector* impulseForward, SoX::Math::Vector* impulseVertical, SoX::Math::Vector* impulseForward2, SoX::Math::Vector* impulse, double delta)
    {
        return GuestToHostFunction<uint32_t>(m_pVftable->fpFunc20, this, impulseForward, impulseVertical, impulseForward2, impulse, delta);
    }

    inline uint32_t IPosturePlugIn::Func24(SoX::Math::Vector* vector, SoX::Math::Quaternion* quat)
    {
        return GuestToHostFunction<uint32_t>(m_pVftable->fpFunc24, this, vector, quat);
    }

    inline void IPosturePlugIn::Func28(SoX::Math::Vector* v1, SoX::Math::Quaternion* n1)
    {
        return GuestToHostFunction<void>(m_pVftable->fpFunc28, this, v1, n1);
    }
}
