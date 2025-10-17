#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/Camera/Cameraman.h>
#include <Sonicteam/Camera/CameraMode.h>

namespace Sonicteam::Camera
{
    class CameraModeManager
    {
    public:
        xpointer<Cameraman> m_pCameraman;
        boost::shared_ptr<CameraMode> m_spCameraMode;
    };
}
