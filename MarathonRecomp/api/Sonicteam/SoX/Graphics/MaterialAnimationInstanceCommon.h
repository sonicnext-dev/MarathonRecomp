#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>
#include <Sonicteam/SoX/Graphics/MaterialAnimationInstance.h>

namespace Sonicteam::SoX::Graphics
{
    class MaterialAnimationInstanceCommon : public MaterialAnimationInstance
    {
    public:
        MARATHON_INSERT_PADDING(0x30);
    };
	
    MARATHON_ASSERT_SIZEOF(MaterialAnimationInstanceCommon, 0x40);
}
