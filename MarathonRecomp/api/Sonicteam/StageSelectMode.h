#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/DocMode.h>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/StageMap.h>

namespace Sonicteam
{
    class StageSelectMode : public SoX::Engine::DocMode
    {
    public:
        xpointer<StageMap> m_RootStageMap;
        xpointer<StageMap> m_CurrentStageMap;
        MARATHON_INSERT_PADDING(0x04);
        be<uint32_t> m_StageMapIndex;
        be<uint32_t> m_StageMapDepth;
        MARATHON_INSERT_PADDING(0x18);
        xpointer<void> m_pMenuListener;
        xpointer<xpointer<const char>> m_StageMapFilter;
        xpointer<uint32_t> m_StageMapType;
        xpointer<const char> m_StageMapName;
        MARATHON_INSERT_PADDING(0x04);
    };

    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_RootStageMap, 0x50);
    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_CurrentStageMap, 0x54);
    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_StageMapIndex, 0x5C);
    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_StageMapDepth, 0x60);
    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_pMenuListener, 0x7C);
    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_StageMapFilter, 0x80);
    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_StageMapType, 0x84);
    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_StageMapName, 0x88);
    MARATHON_ASSERT_SIZEOF(StageSelectMode, 0x90);
}
