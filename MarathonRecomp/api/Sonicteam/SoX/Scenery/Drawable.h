#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Scenery/CameraEventCallback.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Scenery
{
    class Drawable : public RefCountObject, public CameraEventCallback
    {
    public:
        MARATHON_INSERT_PADDING(0x64);
    };
}
