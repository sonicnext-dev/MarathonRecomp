#include <user/achievement_manager.h>

// TODO (Hyper): implement achievements menu.
void AchievementManagerUnlockMidAsmHook(PPCRegister& id)
{
    AchievementManager::Unlock(id.u32);
}
