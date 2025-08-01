#include <api/Marathon.h>
#include <os/logger.h>
#include <user/config.h>
#include <app.h>

// Sonicteam::Player::State::TailsContext::Update
PPC_FUNC_IMPL(__imp__sub_8221A7D8);
PPC_FUNC(sub_8221A7D8)
{
    if (!Config::TailsGauge)
    {
        __imp__sub_8221A7D8(ctx, base);
        return;
    }

    auto pTailsContext = (Sonicteam::Player::State::TailsContext*)g_memory.Translate(ctx.r3.u32);
    auto pPlayer = pTailsContext->m_pScore->m_pPlayer;

    if (auto pGauge = pPlayer->GetGauge<Sonicteam::Player::SonicGauge>())
    {
        pGauge->m_Maximum = 100.0f;
        pGauge->m_Value = (100.0f / pTailsContext->m_FlightDuration) * pTailsContext->m_FlightTime;
    }

    auto pState = pPlayer->m_pStateMachine->GetBase()->m_pState;
    auto pGameImp = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::GameMode>()->m_pGameImp;

    auto maturityValue = 1.0f;

    // Drain maturity value for Sonicteam::Player::State::TailsFlight.
    if ((uint32_t)pState->m_pVftable.get() == 0x82005404)
        maturityValue = (1.0f / pTailsContext->m_FlightLimit) * pState->m_Time;

    pGameImp->m_PlayerData[pPlayer->m_PlayerIndex].MaturityValue = maturityValue;

    __imp__sub_8221A7D8(ctx, base);
}

// IDynamicLink (Context Any)
PPC_FUNC_IMPL(__imp__sub_8220F330);
PPC_FUNC(sub_8220F330)
{
    if (!Config::TailsGauge)
    {
        __imp__sub_8220F330(ctx, base);
        return;
    }

    //Getting vft from r3
    uint32_t vft = PPC_LOAD_U32(ctx.r3.u32);

    //Getting plugin from r4
    if (uint32_t plugin = PPC_LOAD_U32(ctx.r4.u32))
    {
        stdx::string* _plugin_name = (stdx::string*)g_memory.Translate(PPC_LOAD_U32(ctx.r4.u32) + 4);

        //If Plugin == score && vft match TailsContext table(0x8200B7F4)
        //Since we can't use lua for OpenGauge, so leave this option, score_plugin is the way to get ObjectPlayer
        if (*_plugin_name == "score" && vft == 0x8200B7F4)
        {
            *_plugin_name = "score_test_ultra";
            *_plugin_name = "score";
            uint32_t obj_player = PPC_LOAD_U32(plugin + 0x34);
            //boost::shared_ptr<T>
            uint64_t _boost_container = (uint64_t)g_userHeap.Alloc(8);
            *(uint64_t*)(_boost_container) = 0;

            //SonicGauge::ctor 
            uint32_t sonic_gauge_module = GuestToHostFunction<uint32_t>(sub_8223F208, g_userHeap.Alloc(0x64));

            //_boost_container = boost::shared_ptr<T>(sonic_gauge_module)
            GuestToHostFunction<void>(sub_821BEAB0, (uint32_t)_boost_container, sonic_gauge_module);

            //apply SonicGauge to ObjectPlayer (IDynamicLink,IGauge,IVariable....)
            GuestToHostFunction<void>(sub_821BECE0, (uint32_t)obj_player, (uint32_t)_boost_container, (uint32_t)1);   //push_back module

            //Store SonicGauge to ObjectPlayer
            PPC_STORE_U32(obj_player + 0x104, sonic_gauge_module);

            //exchange sp_counted_base ,  boost::detail::sp_counted_base::release((_DWORD **)(v7 + 0x108), _boost_container);
            GuestToHostFunction<void>(sub_821600C0, obj_player + 0x108, (uint32_t)(_boost_container + 4));

            //release sp_counted_base, boost::detail::sp_counted_base::release(_boost_container + 4)
            if (uint32_t sp_detail = *(uint32_t*)(_boost_container + 4))
            {
                GuestToHostFunction<void>(sub_821601B8, (uint32_t)sp_detail);
            }

            g_userHeap.Free((void*)_boost_container);

        }
    }

    __imp__sub_8220F330(ctx, base);
}
