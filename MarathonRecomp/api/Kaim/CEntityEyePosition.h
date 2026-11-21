#pragma once

#include <Marathon.inl>
#include <Kaim/CEntityAttribute.h>
#include <Kaim/CVector.h>

namespace Kaim
{
    class CEntityEyePosition : public CEntityAttribute
    {
    public:
        CVector m_eyePosition;
    };

    MARATHON_ASSERT_OFFSETOF(CEntityEyePosition, m_eyePosition, 0x04);
    MARATHON_ASSERT_SIZEOF(CEntityEyePosition, 0x10);
}
