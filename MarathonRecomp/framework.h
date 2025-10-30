#pragma once

#define PROC_ADDRESS(libraryName, procName) \
    GetProcAddress(LoadLibrary(TEXT(libraryName)), procName)

#define LIB_FUNCTION(returnType, libraryName, procName, ...) \
    typedef returnType _##procName(__VA_ARGS__); \
    _##procName* procName = (_##procName*)PROC_ADDRESS(libraryName, #procName);

#define STR(x) #x

template<typename T>
inline T RoundUp(const T& in_rValue, uint32_t in_round)
{
    return (in_rValue + in_round - 1) & ~(in_round - 1);
}

template<typename T>
inline T RoundDown(const T& in_rValue, uint32_t in_round)
{
    return in_rValue & ~(in_round - 1);
}

inline size_t StringHash(const std::string_view& str)
{
    return XXH3_64bits(str.data(), str.size());
}

template<typename TValue>
constexpr size_t FirstBitLow(TValue value)
{
    constexpr size_t nbits = sizeof(TValue) * 8;
    constexpr auto zero = TValue{};
    constexpr auto one = static_cast<TValue>(1);

    for (size_t i = 0; i < nbits; i++)
    {
        if ((value & (one << i)) != zero)
        {
            return i;
        }
    }

    return 0;
}

inline std::unique_ptr<uint8_t[]> ReadAllBytes(const char* filePath, size_t& fileSize)
{
    FILE* file = fopen(filePath, "rb");

    if (!file)
        return std::make_unique<uint8_t[]>(0);

    fseek(file, 0, SEEK_END);

    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    auto data = std::make_unique<uint8_t[]>(fileSize);
    fread(data.get(), 1, fileSize, file);

    fclose(file);

    return data;
}

inline bool strcmpIgnoreCase(const char* a, const char* b)
{
    for (size_t i = 0; i < strlen(a); i++)
    {
        if (a[i] != '\0' && b[i] == '\0')
            return false;

        if (a[i] == '\0' && b[i] != '\0')
            return false;

        auto c1 = std::tolower((uint8_t)a[i]);
        auto c2 = std::tolower((uint8_t)b[i]);

        if (c1 != c2)
            return false;
    }

    return true;
}

inline bool strcmpWildcard(const char* str, const char* pattern)
{
    // Match if both strings passed all
    // comparisons and reached the end.
    if (*pattern == '\0' && *str == '\0')
        return true;

    // Check if any number of chars matches.
    if (*pattern == '*')
    {
        // Skip duplicates.
        while (*(pattern + 1) == '*')
            pattern++;

        return strcmpWildcard(str, pattern + 1) || (*str && strcmpWildcard(str + 1, pattern));
    }

    // Check if current char matches.
    if (*pattern == '?' || *pattern == *str)
        return strcmpWildcard(str + 1, pattern + 1);

    return false;
}
