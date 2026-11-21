#pragma once

#include <Marathon.inl>
#include <Kaim/CArray.h>
#include <Kaim/CEntityInfo.h>
#include <Kaim/CEntityInfoManager.h>
#include <Kaim/CObject.h>
#include <Kaim/CVarList.h>

namespace Kaim
{
    class CEntity;

    class CEntityProfile : public CObject
    {
    public:
        xpointer<CEntityInfoManager> m_entityInfoManager;
        xpointer<CVarList<xpointer<CEntity>>> m_entities;
        CArray<xpointer<CEntityInfo>> m_infos;
        xpointer<char> m_name;
        be<uint32_t> m_maxCount;
    };

    MARATHON_ASSERT_OFFSETOF(CEntityProfile, m_entityInfoManager, 0x04);
    MARATHON_ASSERT_OFFSETOF(CEntityProfile, m_entities, 0x08);
    MARATHON_ASSERT_OFFSETOF(CEntityProfile, m_infos, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CEntityProfile, m_name, 0x1C);
    MARATHON_ASSERT_OFFSETOF(CEntityProfile, m_maxCount, 0x20);
    MARATHON_ASSERT_SIZEOF(CEntityProfile, 0x24);
}
