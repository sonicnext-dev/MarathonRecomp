#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>
#include <Sonicteam/SoX/Graphics/AnimationCore/CoreTemplate.h>
#include <Sonicteam/SoX/Graphics/MaterialAnimation.h>

namespace Sonicteam::SoX::Graphics
{
    class MaterialAnimationCommon : public MaterialAnimation
    {
    public:
        boost::shared_ptr<AnimationCore::CoreTemplate<MaterialAnimationObject>> m_spMaterialAnimationObject;
    };


    MARATHON_ASSERT_OFFSETOF(MaterialAnimationCommon, m_spMaterialAnimationObject, 0x70);
    MARATHON_ASSERT_SIZEOF(MaterialAnimationCommon, 0x78);



    
}
