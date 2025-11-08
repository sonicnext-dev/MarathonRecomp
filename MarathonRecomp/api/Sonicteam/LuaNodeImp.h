#pragma once

#include <Marathon.inl>
#include <Sonicteam/LuaNode.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class LuaNodeImp : public LuaNode
    {
    public:
        stdx::string m_LuaTable;
    };
}
