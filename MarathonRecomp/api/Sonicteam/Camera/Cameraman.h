#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>

namespace Sonicteam::Camera
{
    class CameraModeManager;

    class Cameraman : public Actor
    {
    public:
        MARATHON_INSERT_PADDING(0x24);
        boost::shared_ptr<CameraModeManager> m_spCameraModeManager;
        MARATHON_INSERT_PADDING(0x2C);
        be<float> m_FOV;
    };
}
