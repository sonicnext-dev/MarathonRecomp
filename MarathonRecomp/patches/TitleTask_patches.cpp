#include <api/Marathon.h>
#include <user/config.h>
#include <user/paths.h>

void RestoreTitleButtons(PPCRegister& val)
{
    val.u32 = 3;
}

void RestoreTitleButtons2(PPCRegister& val)
{
    val.u32 = 5;
}

void SetDefaultTitleTaskSelection(PPCRegister& pThis, PPCRegister& csdIndex)
{
    auto pTitleTask = (Sonicteam::TitleTask*)g_memory.Translate(pThis.u32);
    auto saveFilePath = GetSaveFilePath(false);

    if (!std::filesystem::exists(saveFilePath))
        return;

    csdIndex.u32 = 5;
    pTitleTask->m_SelectedIndex = 1;
}

bool DisableTitleTaskStartWait()
{
    return Config::DisableTitleInputDelay;
}

PPC_FUNC_IMPL(__imp__sub_82511540);
PPC_FUNC(sub_82511540)
{
    if (Config::DisableTitleInputDelay)
    {
        ctx.r3.u32 = 1;
        return;
    }

    __imp__sub_82511540(ctx, base);
}
