#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <lua50/lua.h>
#include <Sonicteam/LuaTableAccess.h>

namespace Sonicteam
{
    class LuaNode
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(0x60);
            be<uint32_t> fpGetTable;
        };

        xpointer<Vftable> m_pVftable;
        xpointer<lua50::lua_State> m_pLuaState;

        boost::shared_ptr<LuaTableAccess> GetTable(const char* in_pName)
        {
            const auto result = guest_stack_var<boost::shared_ptr<LuaTableAccess>>();
            const auto name = guest_stack_var<stdx::string>(in_pName);

            GuestToHostFunction<LuaTableAccess*>(m_pVftable->fpGetTable.get(), result.get(), this, name->c_str());

            return *result;
        }
    };

    MARATHON_ASSERT_OFFSETOF(LuaNode::Vftable, fpGetTable, 0x60);
    MARATHON_ASSERT_SIZEOF(LuaNode::Vftable, 0x64);

    MARATHON_ASSERT_OFFSETOF(LuaNode, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(LuaNode, m_pLuaState, 0x04);
    MARATHON_ASSERT_SIZEOF(LuaNode, 0x08);
}
