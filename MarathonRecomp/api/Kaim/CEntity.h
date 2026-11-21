#pragma once

#include <Marathon.inl>
#include <Kaim/CBrain.h>
#include <Kaim/CEntityAttribute.h>
#include <Kaim/CEntityDefinition.h>
#include <Kaim/CEntityProfile.h>
#include <Kaim/CObject.h>
#include <Kaim/CTeam.h>
#include <Kaim/CVarList.h>
#include <Kaim/CVector.h>
#include <Kaim/CWorld.h>

namespace Kaim
{
    class CEntity : public CObject
    {
    public:
        xpointer<char> m_name;
        CVarList<xpointer<CEntity>>::CIterator m_engineId;
        CVarList<xpointer<CEntity>>::CIterator m_id;
        xpointer<CBrain> m_brain;
        be<uint32_t> m_type;
        xpointer<void> m_userData;
        xpointer<CEntityDefinition> m_definition;
        xpointer<CWorld> m_world;
        xpointer<CVarList<xpointer<CTeam>>> m_teams;
        xpointer<CWorld> m_owner;
        CVector m_position;
        CVector m_oriAngles;
        CVector m_oriVector;
        be<float> m_speed;
        be<int> m_attrCount;
        xpointer<xpointer<CEntityAttribute>> m_attributes;
        xpointer<CEntityProfile> m_profile;
        CVarList<xpointer<CEntity>>::CIterator m_posInProfile;
    };

    MARATHON_ASSERT_OFFSETOF(CEntity, m_name, 0x04);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_engineId, 0x08);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_id, 0x10);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_brain, 0x18);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_type, 0x1C);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_userData, 0x20);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_definition, 0x24);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_world, 0x28);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_teams, 0x2C);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_owner, 0x30);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_position, 0x34);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_oriAngles, 0x40);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_oriVector, 0x4C);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_speed, 0x58);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_attrCount, 0x5C);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_attributes, 0x60);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_profile, 0x64);
    MARATHON_ASSERT_OFFSETOF(CEntity, m_posInProfile, 0x68);
    MARATHON_ASSERT_SIZEOF(CEntity, 0x70);
}
