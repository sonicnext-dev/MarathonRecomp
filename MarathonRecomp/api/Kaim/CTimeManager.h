#pragma once

#include <Marathon.inl>
#include <Kaim/CObject.h>

namespace Kaim
{
    class CTimeManager : public CObject
    {
    public:
        class CEntityInfo : public CObject
        {
        public:
            xpointer<be<float>> m_priorities;
            be<float> m_priority;
            be<float> m_priorityFactor;
            xpointer<be<float>> m_lastAperiodicTaskUse;
            xpointer<be<float>> m_lastPeriodicTaskUse;
            xpointer<be<float>> m_lastAperiodicTaskRequest;
        };

        class CTeamInfo : public CObject
        {
        public:
            xpointer<be<float>> m_lastAperiodicTaskUse;
            xpointer<be<float>> m_lastPeriodicTaskUse;
        };

        be<uint32_t> m_periodicCount;
        be<uint32_t> m_aperiodicCount;
        be<uint32_t> m_estimatedCount;
        xpointer<xpointer<char>> m_periodicTaskNames;
        xpointer<xpointer<char>> m_aperiodicTaskNames;
        xpointer<xpointer<char>> m_estimatedTaskNames;
        be<uint32_t> m_maxAperiodicTaskCount;
        be<uint32_t> m_maxPeriodicTaskCount;
        be<uint32_t> m_maxEstimatedTaskCount;
        be<float> m_kaimTpf;
        xpointer<be<float>> m_taskTpf;
        xpointer<be<int>> m_taskCallMax;
        xpointer<be<float>> m_taskPriority;
        xpointer<be<float>> m_taskPeriodicity;
        xpointer<be<float>> m_taskEstimatedConsumption;
        xpointer<CEntityInfo> m_entityInfo;
        xpointer<CTeamInfo> m_teamInfo;
        xpointer<be<float>> m_lastAperiodicTaskUse;
        be<float> m_currentFrameStart;
        be<float> m_maxFrameTime;
        be<float> m_lastFrameStart;
        be<float> m_frameDeltaTime;
        xpointer<be<float>> m_currentFrameTaskTime;
        xpointer<be<int>> m_currentFrameTaskCell;
        xpointer<be<int>> m_entityOrder;
        be<int> m_nextEntityId;
        bool m_noMoreTime;
        be<int> m_prioritaryEntityId;
        be<float> m_estimatedIntraFrameTime;
        bool m_deterministicMode;
        bool m_warnOnSuspiciousCpuRequests;
    };

    MARATHON_ASSERT_OFFSETOF(CTimeManager::CEntityInfo, m_priorities, 0x04);
    MARATHON_ASSERT_OFFSETOF(CTimeManager::CEntityInfo, m_priority, 0x08);
    MARATHON_ASSERT_OFFSETOF(CTimeManager::CEntityInfo, m_priorityFactor, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CTimeManager::CEntityInfo, m_lastAperiodicTaskUse, 0x10);
    MARATHON_ASSERT_OFFSETOF(CTimeManager::CEntityInfo, m_lastPeriodicTaskUse, 0x14);
    MARATHON_ASSERT_OFFSETOF(CTimeManager::CEntityInfo, m_lastAperiodicTaskRequest, 0x18);
    MARATHON_ASSERT_SIZEOF(CTimeManager::CEntityInfo, 0x1C);

    MARATHON_ASSERT_OFFSETOF(CTimeManager::CTeamInfo, m_lastAperiodicTaskUse, 0x04);
    MARATHON_ASSERT_OFFSETOF(CTimeManager::CTeamInfo, m_lastPeriodicTaskUse, 0x08);
    MARATHON_ASSERT_SIZEOF(CTimeManager::CTeamInfo, 0x0C);

    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_periodicCount, 0x04);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_aperiodicCount, 0x08);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_estimatedCount, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_periodicTaskNames, 0x10);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_aperiodicTaskNames, 0x14);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_estimatedTaskNames, 0x18);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_maxAperiodicTaskCount, 0x1C);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_maxPeriodicTaskCount, 0x20);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_maxEstimatedTaskCount, 0x24);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_kaimTpf, 0x28);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_taskTpf, 0x2C);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_taskCallMax, 0x30);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_taskPriority, 0x34);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_taskPeriodicity, 0x38);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_taskEstimatedConsumption, 0x3C);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_entityInfo, 0x40);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_teamInfo, 0x44);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_lastAperiodicTaskUse, 0x48);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_currentFrameStart, 0x4C);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_maxFrameTime, 0x50);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_lastFrameStart, 0x54);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_frameDeltaTime, 0x58);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_currentFrameTaskTime, 0x5C);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_currentFrameTaskCell, 0x60);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_entityOrder, 0x64);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_nextEntityId, 0x68);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_noMoreTime, 0x6C);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_prioritaryEntityId, 0x70);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_estimatedIntraFrameTime, 0x74);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_deterministicMode, 0x78);
    MARATHON_ASSERT_OFFSETOF(CTimeManager, m_warnOnSuspiciousCpuRequests, 0x79);
    MARATHON_ASSERT_SIZEOF(CTimeManager, 0x7C);
}
