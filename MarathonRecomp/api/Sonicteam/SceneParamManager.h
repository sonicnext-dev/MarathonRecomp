#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/ISceneParam.h>
#include <stdx/map.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class SceneParamManager
    {
    public:
        xpointer<void> m_pVftable;
        xpointer<SoX::Engine::Doc> m_pDoc;
        stdx::map<stdx::string, SoX::RefSharedPointer<ISceneParam>> m_SceneParams;
        MARATHON_INSERT_PADDING(0x14);
        be<uint32_t> m_Field28; // Shader?
        MARATHON_INSERT_PADDING(0x1C);
    };

    MARATHON_ASSERT_OFFSETOF(SceneParamManager, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(SceneParamManager, m_pDoc, 0x04);
    MARATHON_ASSERT_OFFSETOF(SceneParamManager, m_SceneParams, 0x08);
    MARATHON_ASSERT_OFFSETOF(SceneParamManager, m_Field28, 0x28);
    MARATHON_ASSERT_SIZEOF(SceneParamManager, 0x48);
}
