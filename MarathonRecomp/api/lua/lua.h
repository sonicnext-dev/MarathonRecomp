#pragma once

#include <Marathon.inl>

namespace lua50
{
    typedef xpointer<void> lua_State;
    typedef be<uint32_t> lua_CFunction;

    struct luaL_reg
    {
        xpointer<const char> m_pName;
        lua_CFunction m_Func;
    };
}
