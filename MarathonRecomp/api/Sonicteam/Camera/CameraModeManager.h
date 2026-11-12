#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/Camera/Cameraman.h>
#include <Sonicteam/Camera/CameraMode.h>

namespace Sonicteam::Camera
{
    class CameraModeManager
    {
    public:
        struct CameraEvent
        {
            boost::shared_ptr<CameraMode> spCameraMode;
            be<uint32_t> ActorID;
            MARATHON_INSERT_PADDING(0x04);
            be<float> Field10;
        };

        xpointer<Cameraman> m_pCameraman;
        boost::shared_ptr<CameraMode> m_spCameraMode;
        MARATHON_INSERT_PADDING(0x08);
        stdx::vector<CameraEvent> m_CameraEvents;
        MARATHON_INSERT_PADDING(0x04);
    };

    MARATHON_ASSERT_OFFSETOF(CameraModeManager, m_pCameraman, 0x00);
    MARATHON_ASSERT_OFFSETOF(CameraModeManager, m_spCameraMode, 0x04);
    MARATHON_ASSERT_OFFSETOF(CameraModeManager, m_CameraEvents, 0x14);
    MARATHON_ASSERT_SIZEOF(CameraModeManager, 0x28);
}
