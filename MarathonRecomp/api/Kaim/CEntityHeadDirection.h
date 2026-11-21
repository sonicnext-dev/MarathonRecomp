#pragma once

#include <Marathon.inl>
#include <Kaim/CEntityAttribute.h>
#include <Kaim/CVector.h>

namespace Kaim
{
    class CEntityHeadDirection : public CEntityAttribute
    {
    public:
        CVector m_headDirection;
    };

    MARATHON_ASSERT_OFFSETOF(CEntityHeadDirection, m_headDirection, 0x04);
    MARATHON_ASSERT_SIZEOF(CEntityHeadDirection, 0x10);
}
