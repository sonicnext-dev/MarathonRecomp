#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>
#include <stdx/map.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class TextBook;

    class TextBookCategory
    {
    public:
        stdx::string m_Name;
        stdx::list<xpointer<TextBook>> m_TextBooks;
    };

    MARATHON_ASSERT_OFFSETOF(TextBookCategory, m_Name, 0x00);
    MARATHON_ASSERT_OFFSETOF(TextBookCategory, m_TextBooks, 0x1C);
    MARATHON_ASSERT_SIZEOF(TextBookCategory, 0x28);
}
