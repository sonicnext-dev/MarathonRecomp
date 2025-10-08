#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Graphics/IAnimation.h>
#include <Sonicteam/SoX/Graphics/IAnimationBlend.h>


namespace Sonicteam::SoX::Graphics
{
    class MaterialAnimationInstance : public IAnimation, public IAnimationBlend<MaterialAnimationInstance>, public RefCountObject
    {
    };

    MARATHON_ASSERT_SIZEOF(MaterialAnimationInstance, 0x10);
}
