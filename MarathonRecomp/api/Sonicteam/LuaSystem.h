#pragma once

#include <Marathon.inl>
#include <Sonicteam/LuaNodeImp.h>
#include <Sonicteam/SoX/IResource2.h>

namespace Sonicteam
{
    class LuaSystemManager;

    class LuaSystem : public LuaNodeImp, public SoX::IResource2<LuaSystem, LuaSystemManager> {};
}
