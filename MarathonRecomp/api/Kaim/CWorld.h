#pragma once

#include <Marathon.inl>
#include <Kaim/CData.h>
#include <Kaim/CDataReader.h>
#include <Kaim/CObject.h>
#include <Kaim/CTeam.h>
#include <Kaim/CTimeManager.h>
#include <Kaim/CVarList.h>
#include <Kaim/CWorldService.h>

namespace Kaim
{
    class CEntity;

    class CWorld : public CObject
    {
    public:
        xpointer<CDataReader> m_reader;
        xpointer<CData> m_configData;
        be<float> m_ONE_METER;
        be<uint32_t> m_MAX_ENTITY;
        xpointer<CVarList<xpointer<CEntity>>> m_entities;
        be<uint32_t> m_MAX_TEAM;
        xpointer<CVarList<xpointer<CTeam>>> m_teams;
        xpointer<xpointer<CWorldService>> m_services;
        be<uint32_t> m_serviceCount;
        be<uint32_t> m_servicesSize;
        CVarList<xpointer<CWorld>>::CIterator m_index;
        xpointer<CTimeManager> m_timeManager;
        be<int> m_addEntityCbListCount;
        be<int> m_addEntityCbListSize;
        xpointer<xpointer<void>> m_addEntityCbList;
        xpointer<xpointer<void>> m_addEntityCbListParam;
        be<int> m_removeEntityCbListCount;
        be<int> m_removeEntityCbListSize;
        xpointer<xpointer<void>> m_removeEntityCbList;
        xpointer<xpointer<void>> m_removeEntityCbListParam;
    };

    MARATHON_ASSERT_OFFSETOF(CWorld, m_reader, 0x04);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_configData, 0x08);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_ONE_METER, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_MAX_ENTITY, 0x10);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_entities, 0x14);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_MAX_TEAM, 0x18);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_teams, 0x1C);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_services, 0x20);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_serviceCount, 0x24);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_servicesSize, 0x28);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_index, 0x2C);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_timeManager, 0x34);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_addEntityCbListCount, 0x38);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_addEntityCbListSize, 0x3C);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_addEntityCbList, 0x40);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_addEntityCbListParam, 0x44);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_removeEntityCbListCount, 0x48);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_removeEntityCbListSize, 0x4C);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_removeEntityCbList, 0x50);
    MARATHON_ASSERT_OFFSETOF(CWorld, m_removeEntityCbListParam, 0x54);
    MARATHON_ASSERT_SIZEOF(CWorld, 0x58);
}
