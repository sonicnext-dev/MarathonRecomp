#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class TextCard
    {
    public:
        boost::shared_ptr<uint8_t> m_spResource;
        xpointer<const uint16_t> m_pText;
        xpointer<const char> m_pVariables;
    };

    MARATHON_ASSERT_OFFSETOF(TextCard, m_spResource, 0x00);
    MARATHON_ASSERT_OFFSETOF(TextCard, m_pText, 0x08);
    MARATHON_ASSERT_OFFSETOF(TextCard, m_pVariables, 0x0C);
    MARATHON_ASSERT_SIZEOF(TextCard, 0x10);
}
