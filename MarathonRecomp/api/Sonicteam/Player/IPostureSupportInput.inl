#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Math.h>

namespace Sonicteam::Player
{

    inline bool IPostureSupportInput::IsInput()
    {
        return GuestToHostFunction<bool>(this->m_pVftable->fpIsInput, this);
    }

    // Recreated from sub_82201E78
    inline bool IPostureSupportInput::sub_82201E78(SoX::Math::Quaternion rot)
    {
        if (this->m_InputForce <= 0.0)
            return 0;

        auto v1 = SoX::Math::TransformCoord({ 0,0,1,1 }, SoX::Math::MatrixFromQuaternion(rot));
        auto v2 = SoX::Math::TransformCoord({ 0,0,1,1 }, SoX::Math::MatrixFromQuaternion(m_InputRotation));
        return (v2.Dot(v1).X < 0.0);
    }

    // Recreated from sub_82201D88
    inline SoX::Math::Quaternion IPostureSupportInput::sub_82201D88(SoX::Math::Quaternion rot)
    {
        if (this->IsInput() || this->m_InputForce <= 0.0)
            return rot;
        else
        {
            return m_InputRotation;
        }
    }

    // Recreated from sub_82201E00
    inline SoX::Math::Quaternion IPostureSupportInput::sub_82201E00(SoX::Math::Quaternion rot)
    {
        if (this->IsInput())
            return rot;
        else
        {
            return SoX::Math::QuaternionMultiply(m_InputRotation, { 0,0,0,0 });
        }
    }

    // Recreated from sub_82201F28
    inline SoX::Math::Quaternion IPostureSupportInput::sub_82201F28(SoX::Math::Quaternion rot, float precission)
    {

        if (this->IsInput() || this->m_InputForce <= 0.0f)
        {
            return rot;
        }
        else
        {
            auto forward = rot * SoX::Math::Vector::Forward();
            auto forwardInput = this->m_InputRotation * SoX::Math::Vector::Forward();
            auto forwardEnd = Sonicteam::SoX::Math::Vector::Normalize3((forwardInput * precission) + (forward * (1.0 - precission)));
            return SoX::Math::Quaternion::CreateFromVectors(SoX::Math::Vector::Forward(), forwardEnd, SoX::Math::Vector::Up());

        }
    }

    // Recreated from sub_82201BC8
    inline SoX::Math::Quaternion IPostureSupportInput::sub_82201BC8(SoX::Math::Quaternion rot, float impulseForward)
    {
        if (this->IsInput() || this->m_InputForce <= 0.0f)
            return rot;

        if (this->m_RotationMethod == 0)
        {
            auto forward = rot * SoX::Math::Vector::Forward();
            auto forwardInput = m_InputRotation * SoX::Math::Vector::Forward();
            auto forwardOutInput = (SoX::Math::Vector)(forward * 0.5) + (forwardInput * 0.5);
            return SoX::Math::Quaternion::CreateFromVectors(SoX::Math::Vector::Forward(), forwardOutInput, SoX::Math::Vector::Up());
        }
        else if (this->m_RotationMethod == 1)
        {
            auto forwardImpulse = rot * (SoX::Math::Vector::Forward() * impulseForward);
            auto forwardInput = m_InputRotation * (SoX::Math::Vector::Forward() * m_RotationSpeed);
            auto forwardOutInput = forwardImpulse + forwardInput;
            return SoX::Math::Quaternion::CreateFromVectors(SoX::Math::Vector::Forward(), forwardOutInput, SoX::Math::Vector::Up());
        }

        return rot;
    }

};

