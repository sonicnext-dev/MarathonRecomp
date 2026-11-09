#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/Camera/CameraMode.h>
#include <Sonicteam/SoX/Graphics/FrameGP.h>
#include <Sonicteam/SoX/Input/Listener.h>
#include <Sonicteam/SoX/Math/Transform.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

namespace Sonicteam
{
    class KynapseControl;
}

namespace Sonicteam::Camera
{
    class CameraModeManager;

    class Cameraman : public Actor
    {
    public:
        class MyInputObj : public SoX::Input::Listener
        {
        public:
            xpointer<Cameraman> m_pOwner;
        };

        boost::shared_ptr<CameraMode> m_spCameraMode;
        SoX::RefSharedPointer<SoX::Graphics::FrameGP> m_spFrameGP;
        MARATHON_INSERT_PADDING(0x10);
        boost::shared_ptr<MyInputObj> m_spInputObj;
        boost::shared_ptr<CameraModeManager> m_spCameraModeManager;
        be<uint32_t> m_Field84;
        be<uint32_t> m_TargetActorID;
        SoX::Math::Transform m_Transform;
        be<float> m_FOV;
        be<uint32_t> m_PlayerIndex;
        boost::shared_ptr<KynapseControl> m_spKynapseControl;
    };

    MARATHON_ASSERT_OFFSETOF(Cameraman::MyInputObj, m_pOwner, 0x18);
    MARATHON_ASSERT_SIZEOF(Cameraman::MyInputObj, 0x1C);

    MARATHON_ASSERT_OFFSETOF(Cameraman, m_spCameraMode, 0x58);
    MARATHON_ASSERT_OFFSETOF(Cameraman, m_spFrameGP, 0x60);
    MARATHON_ASSERT_OFFSETOF(Cameraman, m_spInputObj, 0x74);
    MARATHON_ASSERT_OFFSETOF(Cameraman, m_spCameraModeManager, 0x7C);
    MARATHON_ASSERT_OFFSETOF(Cameraman, m_Field84, 0x84);
    MARATHON_ASSERT_OFFSETOF(Cameraman, m_TargetActorID, 0x88);
    MARATHON_ASSERT_OFFSETOF(Cameraman, m_Transform, 0x90);
    MARATHON_ASSERT_OFFSETOF(Cameraman, m_FOV, 0xB0);
    MARATHON_ASSERT_OFFSETOF(Cameraman, m_PlayerIndex, 0xB4);
    MARATHON_ASSERT_OFFSETOF(Cameraman, m_spKynapseControl, 0xB8);
    MARATHON_ASSERT_SIZEOF(Cameraman, 0xC0);
}
