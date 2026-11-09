#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/RefLinkNode.h>
#include <Sonicteam/ISceneParam.h>

namespace Sonicteam
{
    class SceneReflectionAreaParam : public ISceneParam
    {
    public:
        SoX::LinkNode<SceneReflectionAreaParam> m_Siblings;
        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_OFFSETOF(SceneReflectionAreaParam, m_Siblings, 0x08);
    MARATHON_ASSERT_SIZEOF(SceneReflectionAreaParam, 0x24);
}
