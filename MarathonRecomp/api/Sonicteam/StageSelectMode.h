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
        xpointer<StageMap> m_pRootStageMap;
        xpointer<StageMap> m_pCurrentStageMap;
        MARATHON_INSERT_PADDING(0x04);
        be<uint32_t> m_StageMapIndex;
        be<uint32_t> m_StageMapDepth;
        MARATHON_INSERT_PADDING(0x18);
        xpointer<void> m_pMenuListener;
        xpointer<xpointer<const char>> m_pStageMapFilter;
        xpointer<uint32_t> m_pStageMapType;
        xpointer<const char> m_pStageMapName;
        MARATHON_INSERT_PADDING(0x04);
    };

    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_pRootStageMap, 0x50);
    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_pCurrentStageMap, 0x54);
    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_StageMapIndex, 0x5C);
    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_StageMapDepth, 0x60);
    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_pMenuListener, 0x7C);
    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_pStageMapFilter, 0x80);
    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_pStageMapType, 0x84);
    MARATHON_ASSERT_OFFSETOF(StageSelectMode, m_pStageMapName, 0x88);
    MARATHON_ASSERT_SIZEOF(StageSelectMode, 0x90);
}
