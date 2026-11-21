#pragma once

#include <Marathon.inl>
#include <Kaim/CArray.h>
#include <Kaim/CEntityInfoFilter.h>
#include <Kaim/CObject.h>
#include <Kaim/CVarList.h>

namespace Kaim
{
    class CEntity;
    class CEntityInfoManager;
    class CEntityProfile;

    class CEntityInfo : public CObject
    {
    public:
        class CExploredEntitiesIterator : public CObject
        {
        public:
            xpointer<CEntityInfo> m_entityInfo;
            be<uint32_t> m_index;
            be<uint32_t> m_currentProfile;
            CVarList<xpointer<CEntity>>::CIterator m_currentEntity;
            bool m_wasReset;
        };

        xpointer<CEntityInfoManager> m_entityInfoManager;
        xpointer<CEntityProfile> m_profile;
        xpointer<char> m_name;
        CExploredEntitiesIterator m_explorationIterator;
        CArray<xpointer<CEntityProfile>> m_exploredProfiles;
        be<float> m_validTime;
        be<float> m_unsafeTime;
        bool m_immediateMode;
        be<float> m_estimatedCpuConsumption;
        be<uint32_t> m_computationCount;
        xpointer<CVarList<xpointer<CEntityInfoFilter>>> m_filters;
    };

    MARATHON_ASSERT_OFFSETOF(CEntityInfo::CExploredEntitiesIterator, m_entityInfo, 0x04);
    MARATHON_ASSERT_OFFSETOF(CEntityInfo::CExploredEntitiesIterator, m_index, 0x08);
    MARATHON_ASSERT_OFFSETOF(CEntityInfo::CExploredEntitiesIterator, m_currentProfile, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CEntityInfo::CExploredEntitiesIterator, m_currentEntity, 0x10);
    MARATHON_ASSERT_OFFSETOF(CEntityInfo::CExploredEntitiesIterator, m_wasReset, 0x18);
    MARATHON_ASSERT_SIZEOF(CEntityInfo::CExploredEntitiesIterator, 0x1C);

    MARATHON_ASSERT_OFFSETOF(CEntityInfo, m_entityInfoManager, 0x04);
    MARATHON_ASSERT_OFFSETOF(CEntityInfo, m_profile, 0x08);
    MARATHON_ASSERT_OFFSETOF(CEntityInfo, m_name, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CEntityInfo, m_explorationIterator, 0x10);
    MARATHON_ASSERT_OFFSETOF(CEntityInfo, m_exploredProfiles, 0x2C);
    MARATHON_ASSERT_OFFSETOF(CEntityInfo, m_validTime, 0x3C);
    MARATHON_ASSERT_OFFSETOF(CEntityInfo, m_unsafeTime, 0x40);
    MARATHON_ASSERT_OFFSETOF(CEntityInfo, m_immediateMode, 0x44);
    MARATHON_ASSERT_OFFSETOF(CEntityInfo, m_estimatedCpuConsumption, 0x48);
    MARATHON_ASSERT_OFFSETOF(CEntityInfo, m_computationCount, 0x4C);
    MARATHON_ASSERT_OFFSETOF(CEntityInfo, m_filters, 0x50);
    MARATHON_ASSERT_SIZEOF(CEntityInfo, 0x54);
}
