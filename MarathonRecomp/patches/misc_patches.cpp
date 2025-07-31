#include <api/Marathon.h>
#include <user/config.h>
#include <user/achievement_manager.h>
#include <api/stdx/string.h>
#include <app.h>


// TODO (Hyper): implement achievements menu.
void AchievementManagerUnlockMidAsmHook(PPCRegister& id)
{
    AchievementManager::Unlock(id.u32);
}

void ContextualHUD_Init()
{
    auto base = g_memory.base;

    static bool Contextual_init;
    if (Contextual_init) return;

    PPC_STORE_U32(0x82036BE4, 0x0); // Sonic
    PPC_STORE_U32(0x82036BE8, 0x3F800000); // Shadow
    PPC_STORE_U32(0x82036BFC, 0x3F800000); // E-123 Omega
    PPC_STORE_U32(0x82036C00, 0x3F800000); // Rouge
    PPC_STORE_U32(0x82036BEC, 0x40000000); // Silver
    PPC_STORE_U32(0x82036BF4, 0x40000000); // Amy
    PPC_STORE_U32(0x82036C04, 0x40000000); // Blaze

    Contextual_init = true;
}

void ContextualHUD_LIFE_BER_ANIME_1(PPCRegister& str, PPCRegister& hud)
{
    if (!Config::RestoreContextualHUDColours) {
        return;
    }

    ContextualHUD_Init();

    enum {
        Sonic,
        Shadow,
        Silver,

        Tails,
        Amy,
        Knuckles,

        Omega,
        Rouge,
        Blaze
    };

    auto base = g_memory.base;
    auto chr_index = PPC_LOAD_U32(hud.u32 + 0x78);

    uint32_t chr_in = str.u32;

    switch (chr_index) {
        case Sonic:
        case Tails:
        case Knuckles:
            chr_in = 0x82036778; // sonic_in
            break;
        case Shadow:
        case Omega:
        case Rouge:
            chr_in = 0x8203676C; // shadow_in
            break;
        case Silver:
        case Amy:
        case Blaze:
            chr_in = 0x82036760; // silver_in
            break;
    }

    str.u32 = chr_in;
}

void ContextualHUD_RING_1(PPCRegister& index, PPCRegister& hud)
{
    if (!Config::RestoreContextualHUDColours) {
        return;
    }

    auto base = g_memory.base;
    auto chr_index = PPC_LOAD_U32(hud.u32 + 0x78);
    index.u32 = chr_index;
}

void PostureDisableEdgeGrabLeftover(PPCRegister& posture) {
    if (!Config::DisableEdgeGrabLeftover) {
        return;
    }

    auto base = g_memory.base;
    *(volatile uint8_t*)(base + (posture.u32 + 0x3C0)) = 1;
}





PPC_FUNC_IMPL(__imp__sub_8221A7D8);
PPC_FUNC(sub_8221A7D8) {

    if (!Config::TailsGauge) {
        __imp__sub_8221A7D8(ctx, base);
        return;
    }

    struct Gauge {
        MARATHON_INSERT_PADDING(0x28);	// IGauge implementation
        be<float> GaugeValue;			// 0x28
        be<float> GaugeGroundTime;		// 0x2C
        be<uint32_t> flags;				// 0x30
        be<uint32_t> IsNotGrounded;		// 0x34
        be<float> c_gauge_max;			// 0x38
    };

    struct UnkPlayerData0xE40 {
        be<uint32_t> ActorID;			//0x0 
        be<uint32_t> RingCount;			//0x4 
        be<uint32_t> unk0xC;			//0x8 
        be<uint32_t> LifeCount;			//0xC 
        be<uint32_t> ScoreCount;		//0x10
        be<float> AliveTime;			//0x14
        be<float> Time; 				//0x18
        be<uint32_t> unk0x1C; 			//0x1C
        be<float> SectionSaveTime; 		//0x20
        be<float> GaugeValue; 			//0x24
        be<uint32_t> MaturityLevel; 	//0x28
        be<float> MaturityValue; 		//0x2C
        be<uint32_t> unk0x30;  			//0x30
        be<uint32_t> unk0x34;			//0x34
        be<uint32_t> SelectedGem;		//0x38
        MARATHON_INSERT_PADDING(0x10);	//0x3C
    };


    //Getting ObjectPlayer From ScorePlugin
    auto score_plugin = PPC_LOAD_U32(ctx.r3.u32 + 0x128);
    auto obj_player = PPC_LOAD_U32(score_plugin + 0x34);

    //Getting StateMachine<T> from ObjectPlayer
    auto obj_player_machine = PPC_LOAD_U32(obj_player + 0xE4) - 0x20; //Cast to StateMashine<T>

    //Getting CurrentState from StateMachine<SoX::AI::State<T>>
    auto obj_player_machine_current_state = PPC_LOAD_U32(obj_player_machine + 4);

    //Getting vft from CurrentState*
    auto obj_player_machine_current_state_vft = PPC_LOAD_U32(obj_player_machine_current_state);

    //Getting GaugeModule From ObjectPlayer
    auto gauge_module = (Gauge*)g_memory.Translate(PPC_LOAD_U32(obj_player + 0x104));

    //ObjectPlayer Index
    auto obj_player_index = (PPC_LOAD_U32(obj_player + 0x98));


    //Getting GameIMP from ApplicationXenon->DocMarathon->DocMode + 0x6C
    auto doc_mode = be<uint32_t>::byteswap(*(uint32_t*)((uint64_t)App::s_pApp->m_pDoc.get() + 8));
    auto game_imp = PPC_LOAD_U32(doc_mode + 0x6C);


    //UnkPlayerData0xE40[4]
    UnkPlayerData0xE40* gameimp_plr_data = (UnkPlayerData0xE40*)g_memory.Translate(game_imp + 0xE40);

    PPCRegister tails_fly_meter;
    PPCRegister c_flight_timer;
    PPCRegister c_flight_timer_b;
    PPCRegister tails_fly_meter_b;

    tails_fly_meter.u32 = PPC_LOAD_U32(ctx.r3.u32 + 0x230);
    c_flight_timer.u32 = PPC_LOAD_U32(ctx.r3.u32 + 0x23C);
    c_flight_timer_b.u32 = PPC_LOAD_U32(ctx.r3.u32 + 0x240);


    //c_flight_timer -> Gauge Meter
    if (gauge_module) {
        float current_meter = (100.0 / c_flight_timer.f32) * tails_fly_meter.f32;
        gauge_module->c_gauge_max = 100.0;
        gauge_module->GaugeValue = current_meter;
    }


    //Check If TailsState == Flight -> Flight to Maturity Meter
    if (obj_player_machine_current_state_vft == 0x82005404) {

        tails_fly_meter_b.u32 = PPC_LOAD_U32(obj_player_machine_current_state + 0x18);
        float current_meter_b = (1.0 / c_flight_timer_b.f32) * tails_fly_meter_b.f32;
        gameimp_plr_data[obj_player_index].MaturityValue = current_meter_b;
    }
    else {
        gameimp_plr_data[obj_player_index].MaturityValue = 1.0;
    }


    __imp__sub_8221A7D8(ctx, base);
}


//IDynamicLink(Context Any)
PPC_FUNC_IMPL(__imp__sub_8220F330);
PPC_FUNC(sub_8220F330)
{
    if (!Config::TailsGauge) {
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
    return;
}
