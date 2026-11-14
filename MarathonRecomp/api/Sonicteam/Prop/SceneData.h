#pragma once

#include <Marathon.inl>
#include <Sonicteam/Prop/SceneDataMgr.h>
#include <Sonicteam/SoX/IResource2.h>

namespace Sonicteam::Prop
{
    class SceneData : public SoX::IResource2<SceneData, SceneDataMgr>
    {
    public:
        MARATHON_INSERT_PADDING(0x1C);
    };

    MARATHON_ASSERT_SIZEOF(SceneData, 0x80);
}
