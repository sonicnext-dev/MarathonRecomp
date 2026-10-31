#pragma once

#include <Marathon.inl>
#include <lua50/lua.h>
#include <Sonicteam/LuaTableAccess.h>

namespace Sonicteam
{
    class LuaTableAccessImp : public LuaTableAccess
    {
    public:
        xpointer<lua50::lua_State> m_pLuaState;
        bool m_IsRoot;
        MARATHON_INSERT_PADDING(0x04);
    };

    MARATHON_ASSERT_OFFSETOF(LuaTableAccessImp, m_pLuaState, 0x04);
    MARATHON_ASSERT_OFFSETOF(LuaTableAccessImp, m_IsRoot, 0x08);
    MARATHON_ASSERT_SIZEOF(LuaTableAccessImp, 0x10);
}
