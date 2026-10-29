#include <api/Marathon.h>
#include <hid/hid.h>
#include <os/locale.h>
#include <patches/aspect_ratio_patches.h>
#include <ui/imgui_utils.h>
#include <user/config.h>

// Sonicteam::SoX::Graphics::Xenon::DeviceXenon::DrawText
PPC_FUNC(sub_8289CB70)
{
    const auto pText = reinterpret_cast<const char*>(base + ctx.r4.u32);

    auto pTextSeek = pText;

    while (*pTextSeek++);

    uint32_t textLengthInChars = pTextSeek - pText;
    uint32_t textLengthInCharsNoTerminator = textLengthInChars - 1;

    if (textLengthInChars > 0x7FFFFFFF || !textLengthInCharsNoTerminator)
        return;

    const auto text = os::locale::ConvertToUTF8(os::locale::Encoding::ShiftJIS, std::string(pText, textLengthInCharsNoTerminator));
    const auto x = float(ctx.r5.u32);
    const auto y = float(ctx.r6.u32);
    const auto colour = reinterpret_cast<Sonicteam::SoX::ARGB8888*>(base + ctx.r7.u32);

    Video::EnqueueImGuiDraw([=]()
    {
        const auto drawList = ImGui::GetBackgroundDrawList();

        drawList->AddText(g_pFntArialUni, Scale(12), { AspectRatioPatches::AlignX(Scale(x)), Scale(y) }, IM_COL32(colour->R(), colour->G(), colour->B(), colour->A()), text.c_str());
    });
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
