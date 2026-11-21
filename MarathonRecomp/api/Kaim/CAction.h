#pragma once

#include <Marathon.inl>
#include <Kaim/CActionAttribute.h>
#include <Kaim/CObject.h>

namespace Kaim
{
    class CAction : public CObject
    {
    public:
        be<int> m_attrCount;
        xpointer<xpointer<CActionAttribute>> m_attributes;
    };

    MARATHON_ASSERT_OFFSETOF(CAction, m_attrCount, 0x04);
    MARATHON_ASSERT_OFFSETOF(CAction, m_attributes, 0x08);
    MARATHON_ASSERT_SIZEOF(CAction, 0x0C);
}
