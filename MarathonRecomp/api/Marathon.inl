#pragma once

#include <cpu/guest_stack_var.h>
#include <kernel/function.h>

constexpr float RAD2DEGf = 57.2958f;
constexpr float DEG2RADf = 0.0174533f;

constexpr double RAD2DEG = 57.29578018188477;
constexpr double DEG2RAD = 0.01745329238474369;

#define MARATHON_CONCAT2(x, y) x##y
#define MARATHON_CONCAT(x, y) MARATHON_CONCAT2(x, y)

#define MARATHON_INSERT_PADDING(length)             \
private:                                            \
    uint8_t MARATHON_CONCAT(pad, __LINE__)[length]; \
public:

#define MARATHON_ASSERT_OFFSETOF(type, field, offset) \
    static_assert(offsetof(type, field) == offset)

#define MARATHON_ASSERT_SIZEOF(type, size) \
    static_assert(sizeof(type) == size)

#define MARATHON_VIRTUAL_FUNCTION(returnType, virtualIndex, ...) \
    GuestToHostFunction<returnType>(*(be<uint32_t>*)(g_memory.Translate(*(be<uint32_t>*)(this) + (4 * virtualIndex))), __VA_ARGS__)

struct MARATHON_NULL_CTOR {};

template <class T, uint32_t pVftable>
class MARATHON_VIRTUAL_BASE : public T
{
public:
    MARATHON_VIRTUAL_BASE()
    {
        *reinterpret_cast<be<uint32_t>*>(this) = pVftable;
    }

    static T::Vftable* GetVftable()
    {
        return static_cast<T::Vftable*>(MmGetHostAddress(pVftable));
    }
};

inline size_t strlenU16(const char16_t* str)
{
    size_t result = 0;
    char16_t c = 0xFFFF;

    while (c != 0)
    {
        c = str[result];
        result++;
    }

    return result;
}

inline bool strcmpU16(const char16_t* a, const char16_t* b, bool endianSwapA = false, bool endianSwapB = false)
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

inline void printU16(const char16_t* str, bool endianSwap = false)
{
    for (size_t i = 0; i < strlenU16(str); i++)
    {
        auto c0 = endianSwap ? ByteSwap(str[i]) >> 8 : str[i] >> 8;
        auto c1 = endianSwap ? ByteSwap(str[i]) & 0xFF : str[i] & 0xFF;

        printf("%c%c", c0, c1);
    }

    printf("\n");
}
