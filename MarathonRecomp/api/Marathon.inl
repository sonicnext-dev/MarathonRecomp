#pragma once

#include <cpu/guest_stack_var.h>
#include <kernel/function.h>

constexpr float RAD2DEGf = 57.2958f;
constexpr float DEG2RADf = 0.0174533f;

constexpr double RAD2DEG = 57.29578018188477;
constexpr double DEG2RAD = 0.01745329238474369;

#define MARATHON_CONCAT2(x, y) x##y
#define MARATHON_CONCAT(x, y) MARATHON_CONCAT2(x, y)

#define MARATHON_INSERT_PADDING(length) \
    uint8_t MARATHON_CONCAT(pad, __LINE__)[length]

#define MARATHON_ASSERT_OFFSETOF(type, field, offset) \
    static_assert(offsetof(type, field) == offset)

#define MARATHON_ASSERT_SIZEOF(type, size) \
    static_assert(sizeof(type) == size)

#define MARATHON_VIRTUAL_FUNCTION(returnType, virtualIndex, ...) \
    GuestToHostFunction<returnType>(*(be<uint32_t>*)(g_memory.Translate(*(be<uint32_t>*)(this) + (4 * virtualIndex))), __VA_ARGS__)

struct MARATHON_NULL_CTOR {};

struct MARATHON_STD_MAP_CONST_CHAR_COMPARE
{
    bool operator()(xpointer<const char> lhs, xpointer<const char> rhs) const
    {
        return std::strcmp(lhs.get(), rhs.get()) < 0;
    }
};

inline std::vector<std::string_view> ParseTextVariables(const char* pVariables)
{
    std::vector<std::string_view> result{};

    if (!pVariables)
        return result;

    auto start = pVariables;
    auto ptr = pVariables;
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

inline std::vector<std::pair<std::string_view, std::string_view>> MapTextVariables(const char* pVariables)
{
    std::vector<std::pair<std::string_view, std::string_view>> result{};

    if (!pVariables)
        return result;

    auto variables = ParseTextVariables(pVariables);

    for (auto& variable : variables)
    {
        auto open = variable.find('(');
        auto close = variable.find(')');

        if (open != std::string_view::npos && close != std::string_view::npos && close > open)
        {
            auto type = variable.substr(0, open);
            auto value = variable.substr(open + 1, close - open - 1);

            result.emplace_back(type, value);
        }
        else
        {
            result.emplace_back(variable, std::string_view{});
        }
    }

    return result;
}

inline size_t strlenU16(const uint16_t* str)
{
    size_t result = 0;
    uint16_t c = 0xFFFF;

    while (c != 0)
    {
        c = str[result];
        result++;
    }

    return result;
}

inline bool strcmpU16(const uint16_t* a, const uint16_t* b, bool endianSwapA = false, bool endianSwapB = false)
{
    for (size_t i = 0; i < strlenU16(a); i += 2)
    {
        auto c1 = endianSwapA ? ByteSwap(a[i]) : a[i];
        auto c2 = endianSwapB ? ByteSwap(b[i]) : b[i];

        if (c1 != 0 && c2 == 0)
            return false;

        if (c1 == 0 && c2 != 0)
            return false;

        if (c1 != c2)
            return false;
    }

    return true;
}

inline void printU16(const uint16_t* str, bool endianSwap = false)
{
    for (size_t i = 0; i < strlenU16(str); i++)
    {
        auto c0 = endianSwap ? ByteSwap(str[i]) >> 8 : str[i] >> 8;
        auto c1 = endianSwap ? ByteSwap(str[i]) & 0xFF : str[i] & 0xFF;

        printf("%c%c", c0, c1);
    }

    printf("\n");
}
