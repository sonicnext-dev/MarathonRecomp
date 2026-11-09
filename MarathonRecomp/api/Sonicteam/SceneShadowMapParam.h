#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/ISceneParam.h>

namespace Sonicteam
{
    class SceneShadowMapParam : public ISceneParam
    {
    public:
        SoX::LinkNode<SceneShadowMapParam> m_Siblings;
    };

    MARATHON_ASSERT_OFFSETOF(SceneShadowMapParam, m_Siblings, 0x08);
    MARATHON_ASSERT_SIZEOF(SceneShadowMapParam, 0x14);
}
