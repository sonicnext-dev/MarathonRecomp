#pragma once

#include <Marathon.inl>
#include <Kaim/CAction.h>
#include <Kaim/CObject.h>

namespace Kaim
{
    class CBrain;

    class CAgent : public CObject
    {
    public:
        xpointer<CAction> m_action;
        xpointer<CBrain> m_brain;
    };

    MARATHON_ASSERT_OFFSETOF(CAgent, m_action, 0x04);
    MARATHON_ASSERT_OFFSETOF(CAgent, m_brain, 0x08);
    MARATHON_ASSERT_SIZEOF(CAgent, 0x0C);
}
