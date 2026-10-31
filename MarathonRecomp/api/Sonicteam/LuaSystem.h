#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource2.h>
#include <Sonicteam/LuaNodeImp.h>
#include <Sonicteam/LuaSystemManager.h>

namespace Sonicteam
{
    class LuaSystem : public LuaNodeImp, public SoX::IResource2<LuaSystem, LuaSystemManager> {};

    MARATHON_ASSERT_SIZEOF(LuaSystem, 0x88);
}
