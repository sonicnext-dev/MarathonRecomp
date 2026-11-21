#pragma once

#include <Marathon.inl>
#include <Kaim/CObject.h>

namespace Kaim
{
    class CActionAttribute : public CObject
    {
    public:
        bool m_upToDate;
    };

    MARATHON_ASSERT_OFFSETOF(CActionAttribute, m_upToDate, 0x04);
    MARATHON_ASSERT_SIZEOF(CActionAttribute, 0x08);
}
