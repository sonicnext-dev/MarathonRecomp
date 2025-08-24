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
