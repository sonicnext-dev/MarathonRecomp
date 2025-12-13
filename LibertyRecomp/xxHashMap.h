#pragma once

struct xxHash
{
    using is_avalanching = void;

    uint64_t operator()(XXH64_hash_t const& x) const noexcept
    {
        return x;
    }
};

template<typename T>
using xxHashMap = ankerl::unordered_dense::map<XXH64_hash_t, T, xxHash>;

inline XXH64_hash_t HashStr(const std::string_view& value)
{
    return XXH3_64bits(value.data(), value.size());
}

template <typename T>
inline T FindHash(const xxHashMap<T>& map, const XXH64_hash_t hash)
{
    auto findResult = map.find(hash);

    if (findResult != map.end())
        return findResult->second;

    return {};
}
