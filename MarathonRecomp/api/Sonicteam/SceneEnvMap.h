#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/ISceneParam.h>
#include <Sonicteam/MyTexture.h>

namespace Sonicteam
{
    class SceneEnvMap : public ISceneParam
    {
    public:
        SoX::LinkNode<SceneEnvMap> m_Siblings;
        xpointer<MyTexture> m_pTexture;
        be<uint32_t> m_Flags;
    };

    MARATHON_ASSERT_OFFSETOF(SceneEnvMap, m_Siblings, 0x08);
    MARATHON_ASSERT_OFFSETOF(SceneEnvMap, m_pTexture, 0x14);
    MARATHON_ASSERT_OFFSETOF(SceneEnvMap, m_Flags, 0x18);
    MARATHON_ASSERT_SIZEOF(SceneEnvMap, 0x1C);
}
