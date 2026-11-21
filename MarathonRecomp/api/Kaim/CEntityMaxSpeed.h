#pragma once

#include <Marathon.inl>
#include <Kaim/CEntityAttribute.h>

namespace Kaim
{
    class CEntityMaxSpeed : public CEntityAttribute
    {
    public:
        be<float> m_maxSpeed;
    };

    MARATHON_ASSERT_OFFSETOF(CEntityMaxSpeed, m_maxSpeed, 0x04);
    MARATHON_ASSERT_SIZEOF(CEntityMaxSpeed, 0x08);
}
