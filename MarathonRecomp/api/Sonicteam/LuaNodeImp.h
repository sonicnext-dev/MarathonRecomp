#pragma once

#include <Marathon.inl>
#include <Sonicteam/LuaNode.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class LuaNodeImp : public LuaNode
    {
    public:
        stdx::string m_Name;
    };

    MARATHON_ASSERT_OFFSETOF(LuaNodeImp, m_Name, 0x08);
    MARATHON_ASSERT_SIZEOF(LuaNodeImp, 0x24);
}
