#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/ISceneParam.h>

namespace Sonicteam
{
    class SceneBloomParam : public ISceneParam
    {
    public:
        SoX::LinkNode<SceneBloomParam> m_Siblings;
        be<float> m_MinThreshold;
        be<float> m_MaxThreshold;
        be<float> m_Scale;
    };

    MARATHON_ASSERT_OFFSETOF(SceneBloomParam, m_Siblings, 0x08);
    MARATHON_ASSERT_OFFSETOF(SceneBloomParam, m_MinThreshold, 0x14);
    MARATHON_ASSERT_OFFSETOF(SceneBloomParam, m_MaxThreshold, 0x18);
    MARATHON_ASSERT_OFFSETOF(SceneBloomParam, m_Scale, 0x1C);
    MARATHON_ASSERT_SIZEOF(SceneBloomParam, 0x20);
}
