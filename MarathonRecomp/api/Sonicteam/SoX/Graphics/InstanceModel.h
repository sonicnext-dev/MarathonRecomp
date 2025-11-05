#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Scenery/CameraEventCallback.h>

namespace Sonicteam::SoX::Graphics
{
    class InstanceModel : public RefCountObject, public Scenery::CameraEventCallback
    {

    };
}
