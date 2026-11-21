#pragma once

#include <Marathon.inl>
#include <Kaim/CObject.h>

namespace Kaim
{
    class CEntityInfoFilter : public CObject
    {
    public:
        xpointer<char> m_name;
    };

    MARATHON_ASSERT_OFFSETOF(CEntityInfoFilter, m_name, 0x04);
    MARATHON_ASSERT_SIZEOF(CEntityInfoFilter, 0x08);
}
