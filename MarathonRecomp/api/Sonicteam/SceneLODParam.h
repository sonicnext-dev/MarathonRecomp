#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/ISceneParam.h>

namespace Sonicteam
{
    class SceneLODParam : public ISceneParam
    {
    public:
        struct LODParam
        {
            bool IsPopulated;
            be<float> Field04;
            be<float> ClumpClipDistance;
            be<float> TerrainClipDistance;
        };

        SoX::LinkNode<SceneLODParam> m_Siblings;
        SoX::Array<LODParam, 19> m_LODParams;
    };

    MARATHON_ASSERT_OFFSETOF(SceneLODParam::LODParam, IsPopulated, 0x00);
    MARATHON_ASSERT_OFFSETOF(SceneLODParam::LODParam, Field04, 0x04);
    MARATHON_ASSERT_OFFSETOF(SceneLODParam::LODParam, ClumpClipDistance, 0x08);
    MARATHON_ASSERT_OFFSETOF(SceneLODParam::LODParam, TerrainClipDistance, 0x0C);
    MARATHON_ASSERT_SIZEOF(SceneLODParam::LODParam, 0x10);

    MARATHON_ASSERT_OFFSETOF(SceneLODParam, m_Siblings, 0x08);
    MARATHON_ASSERT_OFFSETOF(SceneLODParam, m_LODParams, 0x14);
    MARATHON_ASSERT_SIZEOF(SceneLODParam, 0x144);
}
