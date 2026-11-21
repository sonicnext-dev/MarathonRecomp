#pragma once

#include <Marathon.inl>
#include <Kaim/CObject.h>
#include <Kaim/CVector.h>

namespace Kaim
{
    class CPointPair : public CObject
    {
    public:
        CVector m_p1;
        CVector m_p2;
    };

    MARATHON_ASSERT_OFFSETOF(CPointPair, m_p1, 0x04);
    MARATHON_ASSERT_OFFSETOF(CPointPair, m_p2, 0x10);
    MARATHON_ASSERT_SIZEOF(CPointPair, 0x1C);
}
