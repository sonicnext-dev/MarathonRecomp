#pragma once

#include <Marathon.inl>
#include <Kaim/CAction.h>
#include <Kaim/CAgent.h>
#include <Kaim/CBrainService.h>
#include <Kaim/CObject.h>

namespace Kaim
{
    class CEntity;

    class CBrain : public CObject
    {
    public:
        xpointer<xpointer<CAgent>> m_agents;
        be<int> m_agentCount;
        xpointer<xpointer<CBrainService>> m_services;
        be<int> m_serviceCount;
        xpointer<CEntity> m_bot;
        xpointer<CAction> m_action;
    };

    MARATHON_ASSERT_OFFSETOF(CBrain, m_agents, 0x04);
    MARATHON_ASSERT_OFFSETOF(CBrain, m_agentCount, 0x08);
    MARATHON_ASSERT_OFFSETOF(CBrain, m_services, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CBrain, m_serviceCount, 0x10);
    MARATHON_ASSERT_OFFSETOF(CBrain, m_bot, 0x14);
    MARATHON_ASSERT_OFFSETOF(CBrain, m_action, 0x18);
    MARATHON_ASSERT_SIZEOF(CBrain, 0x1C);
}
