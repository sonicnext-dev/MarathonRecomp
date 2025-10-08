#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Math.h>

namespace Sonicteam::Player
{
    class IPostureSupportInput
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpIsInput;
            be<uint32_t> fpDestroy;
        };

        xpointer<Vftable> m_pVftable;
        SoX::Math::Quaternion m_InputRotation;
        be<float> m_InputForce; // [0..1.0]
        be<uint32_t> m_RotationMethod;
        be<float> m_RotationSpeed;

        bool IsInput();

        bool sub_82201E78(SoX::Math::Quaternion rot);

        SoX::Math::Quaternion sub_82201D88(SoX::Math::Quaternion rot);

        SoX::Math::Quaternion sub_82201E00(SoX::Math::Quaternion rot);

        SoX::Math::Quaternion sub_82201F28(SoX::Math::Quaternion rot, float precission);

        SoX::Math::Quaternion sub_82201BC8(SoX::Math::Quaternion rot, float impulseForward);

    };

    MARATHON_ASSERT_SIZEOF(IPostureSupportInput, 48);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportInput, m_InputRotation, 0x10);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportInput, m_InputForce, 0x20);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportInput, m_RotationMethod, 0x24);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportInput, m_RotationSpeed, 0x28);
}

#include "IPostureSupportInput.inl"
