#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/MessageReceiver.h>

namespace Sonicteam::Camera
{
    class CameraMode : public SoX::MessageReceiver
    {
    public:
        MARATHON_INSERT_PADDING(0x24);
    };
}
