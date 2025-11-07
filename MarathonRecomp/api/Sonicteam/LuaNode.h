#pragma once

#include <Marathon.inl>
#include <api/lua/lua.h>

namespace Sonicteam
{

    class LuaNode
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
        };
        xpointer<Vftable> m_pVftable;
        lua50::lua_State m_pLuaState;
    };

    MARATHON_ASSERT_SIZEOF(LuaNode, 8);
}
