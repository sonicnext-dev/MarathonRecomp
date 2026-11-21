#pragma once

#include <Marathon.inl>
#include <Kaim/CObject.h>
#include <Kaim/CVarList.h>

namespace Kaim
{
    class CEntity;
    class CWorld;

    class CTeam : public CObject
    {
    public:
        xpointer<char> m_name;
        xpointer<CVarList<xpointer<CEntity>>> m_entities;
        CVarList<xpointer<CTeam>>::CIterator m_id;
        bool m_active;
        xpointer<CWorld> m_owner;
    };

    MARATHON_ASSERT_OFFSETOF(CTeam, m_name, 0x04);
    MARATHON_ASSERT_OFFSETOF(CTeam, m_entities, 0x08);
    MARATHON_ASSERT_OFFSETOF(CTeam, m_id, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CTeam, m_active, 0x14);
    MARATHON_ASSERT_OFFSETOF(CTeam, m_owner, 0x18);
    MARATHON_ASSERT_SIZEOF(CTeam, 0x1C);
}
