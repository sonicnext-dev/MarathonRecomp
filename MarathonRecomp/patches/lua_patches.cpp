#include <api/Marathon.h>
#include <os/logger.h>

// luaG_errormsg
PPC_FUNC_IMPL(__imp__sub_825E0430);
PPC_FUNC(sub_825E0430)
{
    const auto L = reinterpret_cast<lua50::lua_State*>(base + ctx.r3.u32);

    if (lua50::lua_isstring(L, -1))
        os::logger::Log(lua50::lua_tostring(L, -1), os::logger::ELogType::Error, "Lua");

    __imp__sub_825E0430(ctx, base);
}
