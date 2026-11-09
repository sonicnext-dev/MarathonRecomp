#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/ISceneParam.h>

namespace Sonicteam
{
    class SceneOLSParam : public ISceneParam
    {
    public:
        SoX::LinkNode<SceneOLSParam> m_Siblings;
        SoX::Math::Vector m_SunColor;
        SoX::Math::Vector m_BRay;
        SoX::Math::Vector m_BMie;
        be<float> m_G;
    };

    MARATHON_ASSERT_OFFSETOF(SceneOLSParam, m_Siblings, 0x08);
    MARATHON_ASSERT_OFFSETOF(SceneOLSParam, m_SunColor, 0x20);
    MARATHON_ASSERT_OFFSETOF(SceneOLSParam, m_BRay, 0x30);
    MARATHON_ASSERT_OFFSETOF(SceneOLSParam, m_BMie, 0x40);
    MARATHON_ASSERT_OFFSETOF(SceneOLSParam, m_G, 0x50);
    MARATHON_ASSERT_SIZEOF(SceneOLSParam, 0x60);
}
