#pragma once

#include <Marathon.inl>
#include <Kaim/CEntityAttribute.h>

namespace Kaim
{
    class CEntityWidth : public CEntityAttribute
    {
    public:
        be<float> m_width;
    };

    MARATHON_ASSERT_OFFSETOF(CEntityWidth, m_width, 0x04);
    MARATHON_ASSERT_SIZEOF(CEntityWidth, 0x08);
}
