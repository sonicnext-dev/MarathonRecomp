#include <api/Marathon.h>
#include <ui/dev_title_menu.h>
#include <user/config.h>

// Sonicteam::StageSelectMode::ProcessMessage
PPC_FUNC_IMPL(__imp__sub_824A8FF0);
PPC_FUNC(sub_824A8FF0)
{
    const auto pMessage = reinterpret_cast<DevMessage*>(base + ctx.r4.u32);

    DevTitleMenu::AudioCommon(pMessage);

    __imp__sub_824A8FF0(ctx, base);
}

// Sonicteam::DevTitleMode::ProcessMessage
PPC_FUNC_IMPL(__imp__sub_824A0E38);
PPC_FUNC(sub_824A0E38)
{
    const auto pStateDevTitle = StateDevTitle::GetInstance();
    const auto pMessage = reinterpret_cast<DevMessage*>(base + ctx.r4.u32);

    if (pStateDevTitle->m_pPictureTitle->m_State3 >= 4)
        return;

    if (pMessage->id == 0x10002)
    {
        if (pMessage->id2 == 0x5A || pMessage->id3)
        {
            pStateDevTitle->m_pPictureTitle->m_State3 = 4;
            DevTitleDelay = DevTitleDelayDefault;
        }
        else
        {
            return;
        }
    }

    DevTitleMenu::AudioCommon(pMessage);

    __imp__sub_824A0E38(ctx, base);
}

// Sonicteam::DocMarathonState::StartMainMode
PPC_FUNC_IMPL(__imp__sub_82161AB8);
PPC_FUNC(sub_82161AB8)
{
    if (!Config::BootDirectlyToDevTitle)
    {
        __imp__sub_82161AB8(ctx, base);
        return;
    }
    
    const auto pDoc = reinterpret_cast<Sonicteam::DocMarathonState*>(base + ctx.r3.u32);
    
    GuestToHostFunction<int>(sub_825E9E28, ctx.r3.u32, ctx.r4.u32);

    ctx.r3.u32 = g_memory.MapVirtual(pDoc->SetDocState(StateDevTitle::GetInstance()));
}

// StateDevTitle::Update
PPC_FUNC_IMPL(__imp__sub_82162298);
PPC_FUNC(sub_82162298)
{
    const auto pStateDevTitle = reinterpret_cast<StateDevTitle*>(base + ctx.r3.u32);

    if (ctx.r3.u32 && pStateDevTitle->m_pPictureTitle && pStateDevTitle->m_pPictureTitle->m_State3 > 4)
    {
        DevTitleDelay = std::clamp(DevTitleDelay - 0.016666668f, 0.0f, DevTitleDelay);
    }
    else
    {
        DevTitleDelay = DevTitleDelayDefault;
    }

    if (!GuestToHostFunction<uint32_t>(sub_82581C68, ctx.r4.u32))
    {
        // Wait for transition.
        GuestToHostFunction<uint32_t>(sub_824A0870, pStateDevTitle->m_pPictureTitle.ptr.get(), 0.016666668);

        // Force to wait transition.
        if (pStateDevTitle->m_pPictureTitle->m_State3 != 6)
            return;
    }

    __imp__sub_82162298(ctx, base);
}
