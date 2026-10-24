#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class TextLanguageParameter
    {
    public:
        struct TextLanguageParameterContext
        {
            xpointer<const char> Name;
            xpointer<const char> Suffix;
            xpointer<const char> Next;
        };

        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_SIZEOF(TextLanguageParameter, 0x10);
}
