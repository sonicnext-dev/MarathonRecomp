#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/DocMode.h>
#include <Sonicteam/StageMap.h>

namespace Sonicteam
{
    class StageSelectMode : public SoX::Engine::DocMode
    {
    public:
        xpointer<Sonicteam::StageMap> m_StageMap;
        xpointer<Sonicteam::StageMap> m_CurrentStageMap;
        MARATHON_INSERT_PADDING(4);
        be< uint32_t> m_CurrentStageMapIndex;
        be< uint32_t> m_Deep;
        MARATHON_INSERT_PADDING(0x1C);
        xpointer<xpointer<const char>> m_StageMapInfo;
        xpointer<uint32_t> m_StageMapType;
        xpointer<const char> m_StageMapName;
        MARATHON_INSERT_PADDING(0x14);
    };
}
