#pragma once

#include <Marathon.inl>
#include <Kaim/CArray.h>
#include <Kaim/CEntityInfo.h>
#include <Kaim/CEntityInfoFilter.h>
#include <Kaim/CVector.h>
#include <Kaim/CWorldService.h>

namespace Kaim
{
    class CEntity;
    class CEntityProfile;

    class CEntityInfoManager : public CWorldService
    {
    public:
        struct EntityPos
        {
            xpointer<CEntity> entity;
            be<int> a;
            be<int> b;
        };

        be<float> m_gridPitch;
        be<float> m_gridUpdatePeriod;
        xpointer<EntityPos> m_areas;
        be<float> m_nextUpdateAreaTime;
        bool m_init;
        be<int> m_aMin;
        be<int> m_aMax;
        be<int> m_bMin;
        be<int> m_bMax;
        be<int> m_a;
        be<int> m_b;
        be<uint32_t> m_iMin;
        be<uint32_t> m_iMax;
        be<uint32_t> m_i;
        bool m_negativeInc;
        CVector m_position;
        be<float> m_sqDistMin;
        CArray<xpointer<CEntityProfile>> m_profiles;
        CArray<xpointer<CEntityInfoFilter>> m_filters;
        CArray<xpointer<CEntityInfo>> m_infos;
        be<uint32_t> m_nextComputationId;
        be<uint32_t> m_infoCellCount;
        bool m_forceValidDates;
        be<uint32_t> m_updateTaskId;
        be<uint32_t> m_nextDateId;
        be<float> m_timerAccuracy;
        be<float> m_dateUpdatePeriod;
        xpointer<be<float>> m_dates;
        be<uint32_t> m_dateCount;
        be<uint32_t> m_invalidDateId;
        be<uint32_t> m_currentDateId;
        be<uint32_t> m_bitsPerDateId;
        be<float> m_estimationsReliability;
        be<float> m_lastUpdateDuration;
        be<float> m_dataUpdateLoopTime;
        be<float> m_dataUpdateLoopDuration;
        be<float> m_datesUpdateLoopTime;
        be<float> m_datesUpdateLoopDuration;
        bool m_datesUpdateStatus;
    };

    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager::EntityPos, entity, 0x00);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager::EntityPos, a, 0x04);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager::EntityPos, b, 0x08);
    MARATHON_ASSERT_SIZEOF(CEntityInfoManager::EntityPos, 0x0C);

    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_gridPitch, 0x04);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_gridUpdatePeriod, 0x08);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_areas, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_nextUpdateAreaTime, 0x10);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_init, 0x14);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_aMin, 0x18);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_aMax, 0x1C);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_bMin, 0x20);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_bMax, 0x24);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_a, 0x28);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_b, 0x2C);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_iMin, 0x30);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_iMax, 0x34);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_i, 0x38);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_negativeInc, 0x3C);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_position, 0x40);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_sqDistMin, 0x4C);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_profiles, 0x50);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_filters, 0x60);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_infos, 0x70);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_nextComputationId, 0x80);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_infoCellCount, 0x84);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_forceValidDates, 0x88);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_updateTaskId, 0x8C);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_nextDateId, 0x90);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_timerAccuracy, 0x94);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_dateUpdatePeriod, 0x98);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_dates, 0x9C);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_dateCount, 0xA0);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_invalidDateId, 0xA4);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_currentDateId, 0xA8);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_bitsPerDateId, 0xAC);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_estimationsReliability, 0xB0);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_lastUpdateDuration, 0xB4);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_dataUpdateLoopTime, 0xB8);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_dataUpdateLoopDuration, 0xBC);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_datesUpdateLoopTime, 0xC0);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_datesUpdateLoopDuration, 0xC4);
    MARATHON_ASSERT_OFFSETOF(CEntityInfoManager, m_datesUpdateStatus, 0xC8);
    MARATHON_ASSERT_SIZEOF(CEntityInfoManager, 0xCC);
}
