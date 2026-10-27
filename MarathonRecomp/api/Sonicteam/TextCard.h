#pragma once

#include <string>
#include <vector>
#include <Marathon.inl>

namespace Sonicteam
{
    class TextCard
    {
    public:
        boost::shared_ptr<uint8_t> m_spResource;
        xpointer<const char16_t> m_pText;
        xpointer<const char> m_pVariables;

        static inline auto ParseVariables(const char* in_pVariables)
        {
            std::vector<std::string_view> result{};

            if (!in_pVariables)
                return result;

            auto start = in_pVariables;
            auto ptr = in_pVariables;
            auto depth = 0;

            while (*ptr)
            {
                if (*ptr == '(')
                {
                    depth++;
                }
                else if (*ptr == ')')
                {
                    depth--;
                }
                else if (*ptr == ',' && !depth)
                {
                    result.emplace_back(start, ptr - start);
                    start = ptr + 1;
                }

                ++ptr;
            }

            if (ptr != start)
                result.emplace_back(start, ptr - start);

            return result;
        }

        static inline auto MapVariables(const char* in_pVariables)
        {
            std::vector<std::pair<std::string_view, std::string_view>> result{};

            if (!in_pVariables)
                return result;

            const auto variables = ParseVariables(in_pVariables);

            for (const auto& rVariable : variables)
            {
                auto open = rVariable.find('(');
                auto close = rVariable.find(')');

                if (open != std::string_view::npos && close != std::string_view::npos && close > open)
                {
                    auto type = rVariable.substr(0, open);
                    auto value = rVariable.substr(open + 1, close - open - 1);

                    result.emplace_back(type, value);
                }
                else
                {
                    result.emplace_back(rVariable, std::string_view());
                }
            }

            return result;
        }
    };

    MARATHON_ASSERT_OFFSETOF(TextCard, m_spResource, 0x00);
    MARATHON_ASSERT_OFFSETOF(TextCard, m_pText, 0x08);
    MARATHON_ASSERT_OFFSETOF(TextCard, m_pVariables, 0x0C);
    MARATHON_ASSERT_SIZEOF(TextCard, 0x10);
}
