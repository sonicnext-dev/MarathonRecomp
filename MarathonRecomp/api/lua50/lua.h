#pragma once

#include <Marathon.inl>

namespace lua50
{
    static inline constexpr int LUA_TNONE = -1;
    static inline constexpr int LUA_TNIL = 0;
    static inline constexpr int LUA_TBOOLEAN = 1;
    static inline constexpr int LUA_TLIGHTUSERDATA = 2;
    static inline constexpr int LUA_TNUMBER = 3;
    static inline constexpr int LUA_TSTRING = 4;
    static inline constexpr int LUA_TTABLE = 5;
    static inline constexpr int LUA_TFUNCTION = 6;
    static inline constexpr int LUA_TUSERDATA = 7;
    static inline constexpr int LUA_TTHREAD = 8;
    static inline constexpr int LUA_NUMTYPES = 9;

    using lua_State = xpointer<void>;
    using lua_CFunction = be<uint32_t>;

    struct luaL_reg
    {
        xpointer<const char> name;
        lua_CFunction func;
    };

    inline void* lua_topointer(lua_State* L, int idx)
    {
        return GuestToHostFunction<void*>(sub_825D5800, L, idx);
    }
}
