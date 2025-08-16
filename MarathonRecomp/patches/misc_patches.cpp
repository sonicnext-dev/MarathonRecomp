#include <api/Marathon.h>
#include <user/config.h>
#include <user/achievement_manager.h>
#include <gpu/imgui/imgui_snapshot.h>

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

void PedestrianAnimationLOD(PPCRegister& val) {
    val.u32 = 0;
}


bool DisableHints()
{
    return !Config::Hints;
}
