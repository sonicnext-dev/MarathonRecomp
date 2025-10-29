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
        // TODO: research these fields (processed by 0x8218C100).
        struct UnknownStruct
        {
            MARATHON_INSERT_PADDING(0x14);
        };

        xpointer<Cameraman> m_pCameraman;
        boost::shared_ptr<CameraMode> m_spCameraMode;
        MARATHON_INSERT_PADDING(8);
        stdx::vector<UnknownStruct> m_vUnkStructs;
    };
}
