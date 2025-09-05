#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/SoX/MessageReceiver.h>
#include <Sonicteam/GameImp.h>

namespace Sonicteam::Camera
{
    class CameraMode : public SoX::MessageReceiver
    {
    public:
        boost::shared_ptr<GameImp> m_spGameImp;
        xpointer<void> m_pTPCameraInputListener;
        MARATHON_INSERT_PADDING(0x18);
    };
}
