#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <api/stdx/vector.h>

namespace Sonicteam::Camera
{
    class Cameraman;
    class CameraMode;
    class CameraModeManager
    {
        //TODO figure out all these, 8218C100
        struct _vector_struct_
        {
            MARATHON_INSERT_PADDING(0x14);
        };
    public:
        xpointer<Cameraman> m_pCameraman;
        boost::shared_ptr<CameraMode> m_spCameraMode;
        MARATHON_INSERT_PADDING(0x8);
        std::vector<_vector_struct_> m_vVector;
    };
}
