#include <api/Marathon.h>
#include <os/logger.h>
#include <app.h>

// luaG_errormsg
PPC_FUNC_IMPL(__imp__sub_825E0430);
PPC_FUNC(sub_825E0430)
{
    const auto L = reinterpret_cast<lua50::lua_State*>(base + ctx.r3.u32);

    if (lua50::lua_isstring(L, -1))
        os::logger::Log(lua50::lua_tostring(L, -1), os::logger::ELogType::Error, "Lua");

    __imp__sub_825E0430(ctx, base);
}

// Sonicteam::LuaSystem::Callback
PPC_FUNC_IMPL(__imp__sub_8263AEE0);
PPC_FUNC(sub_8263AEE0)
{
    auto pGame = App::s_pApp->GetGame();

    if (pGame != nullptr)
    {
        auto pMissionVm = pGame->m_pMissionCore->m_pVirtualMachine;
        auto pFuncName = reinterpret_cast<const char *>(base + ctx.r4.u32);
        auto pEventName = reinterpret_cast<const char *>(base + ctx.r6.u32);

        auto msgGetGlobalFlag = guest_stack_var<Sonicteam::Message::Mission::MsgGetGlobalFlag>(6002);
        pGame->m_pMissionCore->ProcessMessage(msgGetGlobalFlag.get());

        // Radical Train mirror checks a special temp flag that is only set if you
        // buy the bounce bracelet in that mission. If you cheat in the bracelet instead
        // it becomes impossible to set this flag. This patch effectively redirects it
        // to check global flag 6002, which is always indicative of bounce bracelet state.
        if (strcmp(pFuncName, "on_event") == 0 && strcmp(pEventName, "goto_rct") == 0
            && pMissionVm->m_MissionName == "scripts/mission/0007/mission.lua"
            && msgGetGlobalFlag->FlagValue == 1)
        {
            // Set c_flag_0507 to 1
            pMissionVm->m_TemporaryFlags[7] = 1;
        }
    }

    __imp__sub_8263AEE0(ctx, base);
}
