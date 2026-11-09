#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/ISceneParam.h>

namespace Sonicteam
{
    class SceneKhronosParam : public ISceneParam
    {
    public:
        SoX::LinkNode<SceneKhronosParam> m_Siblings;
        be<float> m_Time;
        SoX::Array<be<float>, 3> m_Values;
    };

    MARATHON_ASSERT_OFFSETOF(SceneKhronosParam, m_Siblings, 0x08);
    MARATHON_ASSERT_OFFSETOF(SceneKhronosParam, m_Time, 0x14);
    MARATHON_ASSERT_OFFSETOF(SceneKhronosParam, m_Values, 0x18);
    MARATHON_ASSERT_SIZEOF(SceneKhronosParam, 0x24);
}
