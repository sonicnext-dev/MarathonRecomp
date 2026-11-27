#include "aspect_ratio_patches.h"
#include <gpu/video.h>
#include <hid/hid.h>
#include <patches/csd_modifier.h>
#include <patches/csd_modifier_manager.h>
#include <patches/hook_event.h>
#include <patches/loading_patches.h>
#include <patches/MainMenuTask_patches.h>
#include <ui/achievement_menu.h>
#include <ui/black_bar.h>
#include <ui/game_window.h>
#include <ui/imgui_utils.h>
#include <ui/options_menu.h>
#include <user/config.h>
#include <app.h>

static float g_aspectRatio;
static float g_aspectRatioOffsetX;
static float g_aspectRatioOffsetY;
static float g_aspectRatioMultiplayerOffsetX;
static float g_aspectRatioScale;
static float g_aspectRatioGameplayScale;
static float g_aspectRatioNarrowScale;
static float g_aspectRatioNarrowMargin;
static float g_horzCentre;
static float g_vertCentre;
static float g_radarMapScale;

constexpr float CHEVRON_INTRO_DURATION = 0.083f;
constexpr float CHEVRON_OUTRO_DURATION = 2.01666666666667f;

static bool g_isMoviePlaying{};

static CsdModifier* g_sceneModifier{};
static CsdModifier* g_castNodeModifier{};
static CsdModifier* g_castModifier{};

static float g_fontPictureWidth{};
static float g_fontPictureHeight{};

static std::array<float, 8> g_csdCorners{};
static bool g_csdCornerExtract{};

static float g_radarMapX{};
static float g_radarMapY{};
static float g_radarMapCoverWidth{};
static float g_radarMapCoverHeight{};

static float g_podBaseRightX{};

static float g_bgArrowsEnd{};
static float g_fgArrowsEnd{};
static float g_chevronLoopTime{};

struct ChevronAnim
{
    uint8_t EndAlpha{};

    float IntroDuration{};
    float IntroStartTime{};

    float OutroDuration{};
    float OutroStartTime{};

    float TimeElapsed{};

    uint8_t CurrentAlpha{};
    float CurrentOffsetX{};

    void Reset()
    {
        TimeElapsed = 0.0f;
        CurrentAlpha = 0;
    }

    void Update(float deltaTime)
    {
        TimeElapsed += deltaTime;

        if (TimeElapsed < IntroStartTime && TimeElapsed < OutroStartTime)
        {
            CurrentAlpha = std::lerp(0, EndAlpha, std::clamp((TimeElapsed - IntroStartTime) / IntroDuration, 0.0f, 1.0f));
        }
        else
        {
            CurrentAlpha = std::lerp(EndAlpha, 0, std::clamp((TimeElapsed - OutroStartTime) / OutroDuration, 0.0f, 1.0f));
        }

        CurrentOffsetX = std::lerp(0.0f, 5.0f, std::clamp(TimeElapsed / g_chevronLoopTime, 0.0f, 1.0f));

        if (TimeElapsed < g_chevronLoopTime)
            return;

        Reset();
    }
};

static std::unordered_map<int32_t, ChevronAnim> g_bgArrows{};
static std::unordered_map<int32_t, ChevronAnim> g_fgArrows{};

static class ChevronAnimResetEvent : public ContextHookEvent<Sonicteam::MainMenuTask>
{
    float m_chevronAspectRatio{};

public:
    void Update(Sonicteam::MainMenuTask* pThis, float deltaTime) override
    {
        if (g_aspectRatio <= WIDE_ASPECT_RATIO)
            return;

        auto aspectRatioChanged = false;

        if (g_aspectRatio != m_chevronAspectRatio)
            aspectRatioChanged = true;

        m_chevronAspectRatio = g_aspectRatio;

        if (pThis->m_State == Sonicteam::MainMenuTask::MainMenuState_MainMenuBack ||
            pThis->m_State == Sonicteam::MainMenuTask::MainMenuState_AudioRoom    ||
            pThis->m_State == Sonicteam::MainMenuTask::MainMenuState_TheaterRoom  ||
            aspectRatioChanged)
        {
            g_bgArrows.clear();
            g_fgArrows.clear();
        }

        for (auto& arrow : g_bgArrows)
            arrow.second.Update(deltaTime);

        for (auto& arrow : g_fgArrows)
            arrow.second.Update(deltaTime);
    }
}
g_chevronAnimResetEvent{};

float ComputeScale(float aspectRatio)
{
    return (aspectRatio * 720.0f) / 1280.0f;
}

void AspectRatioPatches::Init()
{
    CsdModifierManager::Read();
    MainMenuTaskPatches::s_events.push_back(&g_chevronAnimResetEvent);
}

void AspectRatioPatches::ComputeOffsets()
{
    float width = Video::s_viewportWidth;
    float height = Video::s_viewportHeight;

    g_aspectRatio = width / height;
    g_aspectRatioGameplayScale = 1.0f;

    if (g_aspectRatio >= NARROW_ASPECT_RATIO)
    {
        g_aspectRatioOffsetX = (width - height * WIDE_ASPECT_RATIO) / 2.0f;
        g_aspectRatioOffsetY = 0.0f;
        g_aspectRatioScale = height / 720.0f;

        // Keep same scale at 16:10.
        if (g_aspectRatio < WIDE_ASPECT_RATIO)
        {
            const auto narrowScale = ComputeScale(NARROW_ASPECT_RATIO);
            const auto lerpFactor = std::clamp((g_aspectRatio - NARROW_ASPECT_RATIO) / (WIDE_ASPECT_RATIO - NARROW_ASPECT_RATIO), 0.0f, 1.0f);

            g_aspectRatioGameplayScale = std::lerp(narrowScale, 1.0f, lerpFactor);
        }
    }
    else
    {
        // 4:3 crop.
        g_aspectRatioOffsetX = (width - width * NARROW_ASPECT_RATIO) / 2.0f;
        g_aspectRatioOffsetY = (height - width / NARROW_ASPECT_RATIO) / 2.0f;
        g_aspectRatioScale = width / 960.0f;
        g_aspectRatioGameplayScale = ComputeScale(NARROW_ASPECT_RATIO);
    }

    g_aspectRatioMultiplayerOffsetX = g_aspectRatioOffsetX / 2.0f;
    g_aspectRatioNarrowScale = std::clamp((g_aspectRatio - NARROW_ASPECT_RATIO) / (WIDE_ASPECT_RATIO - NARROW_ASPECT_RATIO), 0.0f, 1.0f);
    g_aspectRatioNarrowMargin = std::lerp(50.0f, 0.0f, g_aspectRatioNarrowScale);
    g_horzCentre = g_aspectRatioOffsetX + 640.0f * (1.0f - g_aspectRatioGameplayScale) * g_aspectRatioScale;
    g_vertCentre = g_aspectRatioOffsetY + 360.0f * (1.0f - g_aspectRatioGameplayScale) * g_aspectRatioScale;
    g_radarMapScale = 256 * g_aspectRatioScale * g_aspectRatioGameplayScale;
}

float AspectRatioPatches::AlignX(float in_offset)
{
    if (Config::UIAlignmentMode == EUIAlignmentMode::Centre && g_aspectRatioOffsetX > 0)
        return g_aspectRatioOffsetX + in_offset;

    return in_offset;
}

float AspectRatioPatches::AlignY(float in_offset)
{
    if (Config::UIAlignmentMode == EUIAlignmentMode::Centre && g_aspectRatioOffsetY > 0)
        return g_aspectRatioOffsetY + in_offset;

    return in_offset;
}

// Chao::CSD::Scene::Render
PPC_FUNC_IMPL(__imp__sub_828C8F60);
PPC_FUNC(sub_828C8F60)
{
    const auto pScene = reinterpret_cast<Chao::CSD::Scene*>(base + ctx.r3.u32);

    g_sceneModifier = CsdModifierManager::Find(ctx.r3.u32);

    if (g_sceneModifier)
    {
        const auto flags = g_sceneModifier->GetFlags();

        pScene->FPS = ((flags & CSD_SCENE_DISABLE_MOTION) != 0) ? 0 : 60;

        if (g_aspectRatio > WIDE_ASPECT_RATIO)
        {
            if ((flags & (CSD_OFFSET_SCALE_LEFT | CSD_OFFSET_SCALE_RIGHT | CSD_CORNER_EXTRACT)) != 0)
            {
                auto r3 = ctx.r3;
                auto r4 = ctx.r4;
                auto r5 = ctx.r5;
                auto r6 = ctx.r6;

                // Queue draw calls, but don't actually draw anything.
                // We just want to extract the corner.
                g_csdCornerExtract = true;
                __imp__sub_828C8F60(ctx, base);
                g_csdCornerExtract = false;

                ctx.r3 = r3;
                ctx.r4 = r4;
                ctx.r5 = r5;
                ctx.r6 = r6;
            }
        }
    }

    __imp__sub_828C8F60(ctx, base);
}

void RenderCsdCastNodeMidAsmHook(PPCRegister& r10, PPCRegister& r27)
{
    g_castNodeModifier = CsdModifierManager::Find(r10.u32 + r27.u32);
}

void RenderCsdCastMidAsmHook(PPCRegister& r4)
{
    g_castModifier = CsdModifierManager::Find(r4.u32);
}

static void Draw(PPCContext& ctx, uint8_t* base, PPCFunc* original, uint32_t stride)
{
    CsdModifier* modifier{};

    if (g_castModifier)
    {
        modifier = g_castModifier;
    }
    else if (g_castNodeModifier)
    {
        modifier = g_castNodeModifier;
    }
    else if (g_sceneModifier)
    {
        modifier = g_sceneModifier;
    }

    if (!modifier)
    {
        static CsdModifier defaultModifier{};

        modifier = &defaultModifier;
    }

    const auto flags = modifier->GetFlags();

    const auto vpWidth = float(Video::s_viewportWidth);
    const auto vpHeight = float(Video::s_viewportHeight);

    if (!App::s_isDrawUI)
    {
        // Hide radar map... elegantly...
        if ((flags & CSD_RADARMAP) != 0)
            g_radarMapX = vpWidth * 2;

        return;
    }

    if (g_csdCornerExtract)
    {
        if ((flags & (CSD_STORE_LEFT_CORNER | CSD_STORE_RIGHT_CORNER)) != 0)
        {
            uint32_t vertexIndex = ((flags & CSD_STORE_LEFT_CORNER) != 0) ? 0 : 3;
            g_csdCorners[modifier->CornerIndex] = *reinterpret_cast<be<float>*>(base + ctx.r4.u32 + vertexIndex * stride);
        }

        return;
    }

    // Hide original button window whilst the options menu is visible.
    if ((flags & CSD_BUTTON_WINDOW) != 0 && MainMenuTaskPatches::s_hideButtonWindow)
        return;

    // Hide life counters when lives are disabled.
    if ((flags & CSD_LIFE_COUNTER) != 0 && Config::DisableLives)
        return;

    CsdVertexModifierGroup* baseVertexModifiers{};
    CsdVertexModifierGroup* repeatVertexModifiers{};

    if (g_aspectRatio > WIDE_ASPECT_RATIO)
    {
        auto& rAlignment = Config::UIAlignmentMode == EUIAlignmentMode::Edge
            ? modifier->Ultrawide.Edge
            : modifier->Ultrawide.Center;

        baseVertexModifiers = &rAlignment.Base;
        repeatVertexModifiers = &rAlignment.Repeat;
    }
    else if (g_aspectRatio < WIDE_ASPECT_RATIO)
    {
        auto& rAlignment = Config::UIAlignmentMode == EUIAlignmentMode::Edge
            ? modifier->Narrow.Edge
            : modifier->Narrow.Center;

        baseVertexModifiers = &rAlignment.Base;
        repeatVertexModifiers = &rAlignment.Repeat;
    }
    else
    {
        static CsdVertexModifierGroup defaultVertexModifiers{};

        baseVertexModifiers = &defaultVertexModifiers;
        repeatVertexModifiers = &defaultVertexModifiers;
    }
    
    if ((flags & CSD_SKIP) != 0)
    {
        if ((flags & CSD_CHEVRON) != 0)
        {
            // Don't draw non-extended arrows at ultrawide.
            if (g_aspectRatio > WIDE_ASPECT_RATIO)
                return;
        }
        else
        {
            // Skip drawing this cast.
            return;
        }
    }

    // Draw black bars if this cast is drawn.
    if ((flags & CSD_BLACK_BAR) != 0)
        BlackBar::Show();

    // Prohibit black bars from being drawn if this cast is drawn.
    if ((flags & CSD_PROHIBIT_BLACK_BAR) != 0)
        BlackBar::Hide();

    // Reserve stack space for vertices.
    const auto size = ctx.r5.u32 * stride;
    ctx.r1.u32 -= (size + 0x0F) & ~0x0F;

    // Copy vertices to stack.
    const auto stack = base + ctx.r1.u32;
    memcpy(stack, base + ctx.r4.u32, size);

    const auto getVertex = [&](const size_t in_index)
    {
        return reinterpret_cast<Chao::CSD::Vertex*>(stack + (in_index * stride));
    };

    auto offsetX = 0.0f;
    auto offsetY = 0.0f;
    auto pivotX = 0.0f;
    auto pivotY = 0.0f;
    auto scaleX = 1.0f;
    auto scaleY = 1.0f;

    const auto needsStretch = g_aspectRatio >= WIDE_ASPECT_RATIO;

    if (needsStretch && (flags & CSD_STRETCH_HORIZONTAL) != 0)
    {
        scaleX = vpWidth / 1280.0f;
    }
    else
    {
        scaleX = g_aspectRatioScale;

        if (needsStretch && (flags & CSD_UNSTRETCH_HORIZONTAL) != 0)
        {
            pivotX = getVertex(CSD_TL)->X;
            offsetX = pivotX * vpWidth / 1280.0f;
        }
        else
        {
            if ((flags & CSD_ALIGN_RIGHT) != 0)
            {
                offsetX = g_aspectRatioOffsetX * 2.0f;

                if ((flags & CSD_MULTIPLAYER) != 0 && g_aspectRatio > WIDE_ASPECT_RATIO)
                    offsetX = g_aspectRatioMultiplayerOffsetX * 2.0f;
            }
            else if ((flags & CSD_ALIGN_LEFT) == 0)
            {
                offsetX = g_aspectRatioOffsetX;
            }

            if ((flags & CSD_CROP_TO_FIT) != 0)
            {
                scaleX = std::max(vpWidth / 1280.0f, vpHeight / 720.0f);

                if ((flags & CSD_ALIGN_RIGHT) != 0)
                {
                    offsetX = vpWidth - (1280.0f * scaleX);
                }
                else if ((flags & CSD_ALIGN_LEFT) != 0)
                {
                    offsetX = 0.0f;
                }
                else
                {
                    offsetX = (vpWidth - (1280.0f * scaleX)) * 0.5f;
                }

                offsetX += pivotX * scaleX;
            }
            else
            {
                const auto fitScale = std::min((vpWidth / 1280.0f) / g_aspectRatioScale, 1.0f);

                scaleX *= fitScale;

                if ((flags & CSD_ALIGN_RIGHT) != 0)
                {
                    offsetX += 1280.0f * (1.0f - fitScale) * g_aspectRatioScale;
                }
                else if ((flags & CSD_ALIGN_LEFT) == 0)
                {
                    offsetX += g_aspectRatioOffsetX + 640.0f * (1.0f - fitScale) * g_aspectRatioScale - g_aspectRatioOffsetX;
                }

                offsetX += pivotX * g_aspectRatioScale;
            }

            // Use camera aspect ratio to correct 3D CSD scenes,
            // rather than viewport aspect ratio.
            if ((flags & CSD_3D) != 0 && g_aspectRatio > WIDE_ASPECT_RATIO)
            {
                if (const auto pApp = App::s_pApp; const auto pGame = pApp->GetGame())
                {
                    if (const auto pMainCamera = pGame->GetCamera("Main"))
                        offsetX = g_aspectRatioOffsetX * (pMainCamera->m_AspectRatio.X / pMainCamera->m_AspectRatio.Y / g_aspectRatio);
                }
            }
        }
    }

    if ((flags & CSD_STRETCH_VERTICAL) != 0)
    {
        scaleY = vpHeight / 720.0f;
    }
    else
    {
        scaleY = g_aspectRatioScale;

        if ((flags & CSD_ALIGN_BOTTOM) != 0)
        {
            offsetY = g_aspectRatioOffsetY * 2.0f;
        }
        else if ((flags & CSD_ALIGN_TOP) == 0)
        {
            offsetY = g_aspectRatioOffsetY;
        }

        if ((flags & CSD_CROP_TO_FIT) != 0)
        {
            scaleY = std::max(vpWidth / 1280.0f, vpHeight / 720.0f);

            if ((flags & CSD_ALIGN_BOTTOM) != 0)
            {
                offsetY = vpHeight - (720.0f * scaleY);
            }
            else if ((flags & CSD_ALIGN_TOP) != 0)
            {
                offsetY = 0.0f;
            }
            else
            {
                offsetY = (vpHeight - (720.0f * scaleY)) * 0.5f;
            }

            offsetY += pivotY * scaleY;
        }
        else
        {
            const auto fitScale = std::min((vpWidth / 1280.0f) / g_aspectRatioScale, 1.0f);

            scaleY *= fitScale;

            if ((flags & CSD_ALIGN_BOTTOM) != 0)
            {
                offsetY += 720.0f * (1.0f - fitScale) * g_aspectRatioScale;
            }
            else if ((flags & CSD_ALIGN_TOP) == 0)
            {
                offsetY += g_aspectRatioOffsetY + 360.0f * (1.0f - fitScale) * g_aspectRatioScale - g_aspectRatioOffsetY;
            }

            offsetY += pivotY * g_aspectRatioScale;
        }
    }

    if (g_aspectRatio > WIDE_ASPECT_RATIO && modifier->CornerMax)
    {
        const auto corner = g_csdCorners[modifier->CornerIndex];

        if ((flags & CSD_OFFSET_SCALE_LEFT) != 0)
        {
            // Offset X by progress of current corner to corner max.
            offsetX *= corner / modifier->CornerMax;
        }
        else if ((flags & CSD_OFFSET_SCALE_RIGHT) != 0)
        {
            offsetX = vpWidth - (vpWidth - offsetX) * (1280.0f - corner) / (1280.0f - modifier->CornerMax);
        }
    }

    auto firstX = 0.0f;
    auto firstY = 0.0f;
    auto lastX = 0.0f;
    auto lastY = 0.0f;
    auto width = 0.0f;
    auto height = 0.0f;

    for (size_t i = 0; i < ctx.r5.u32; i++)
    {
        const auto vertex = getVertex(i);

        auto x = offsetX + (vertex->X - pivotX) * scaleX;
        auto y = offsetY + (vertex->Y - pivotY) * scaleY;

        if ((flags & CSD_EXTEND_LEFT) != 0 && (i == 0 || i == 1))
        {
            x = std::lerp(vpWidth, std::min(x, 0.0f), baseVertexModifiers->ExtendBias.X);
        }
        else if ((flags & CSD_EXTEND_RIGHT) != 0 && (i == 2 || i == 3))
        {
            // Preserve Audio Room "pod" base right edge.
            if ((flags & CSD_POD_BASE) != 0)
                g_podBaseRightX = x;

            x = std::lerp(0.0f, std::max(x, vpWidth), baseVertexModifiers->ExtendBias.X);
        }

        switch (i)
        {
            case 0: firstX = x; break;
            case 1: lastY  = y; break;
            case 2: firstY = y; break;
            case 3: lastX  = x; break;
        }

        if ((flags & CSD_RADARMAP) != 0)
        {
            g_radarMapX = x;
            g_radarMapY = y;
        }

        vertex->X = x;
        vertex->Y = y;
    }

    width = fabs(lastX - firstX);
    height = fabs(lastY - firstY);

    auto tl = CSD_TL;
    auto bl = CSD_BL;
    auto tr = CSD_TR;
    auto br = CSD_BR;

    // Swap left and right vertices.
    if (lastX < firstX)
    {
        tl = CSD_TR;
        bl = CSD_BR;
        tr = CSD_TL;
        br = CSD_BL;
    }

    // Swap top and bottom vertices.
    if (lastY < firstY)
    {
        const auto lastTL = tl;
        const auto lastBL = bl;
        const auto lastTR = tr;
        const auto lastBR = br;

        tl = lastBL;
        bl = lastTL;
        tr = lastBR;
        br = lastTR;
    }

    if ((flags & CSD_RADARMAP) != 0)
    {
        g_radarMapCoverWidth = width;
        g_radarMapCoverHeight = height;
    }

    auto targetScaleX = 1.0f;
    auto targetScaleY = 1.0f;

    if (App::s_pApp && App::s_pApp->m_pDevice)
    {
        const auto pDevice = static_cast<GuestDevice*>(App::s_pApp->m_pDevice.get());
        const auto pFrameBufferObject = App::s_pApp->m_pDoc->m_pMyGraphicsDevice->m_spFrameBufferObject.get();

        targetScaleX = float(pDevice->viewport.width) / vpWidth;
        targetScaleY = float(pDevice->viewport.height) / vpHeight;

        if (pFrameBufferObject && pFrameBufferObject->m_FrameBufferSurfaces[0])
        {
            const auto fboName = std::string_view(pFrameBufferObject->m_FrameBufferSurfaces[0]->m_MgrResourceName.c_str());

            if (fboName == "backbuffer")
            {
                // Always unstretch backbuffer CSD draws.
                targetScaleX = 1.0f;
                targetScaleY = 1.0f;
            }
            else if (fboName.starts_with("coframebuffer"))
            {
                // coframebufferN is stretched 2x wide on the x-axis.
                // Following plain viewport will squash CSD.
                targetScaleX *= 2;
            }
        }
    }

    const auto r3 = ctx.r3;
    const auto r5 = ctx.r5;
    const auto needsTargetScale = targetScaleX != 1.0f || targetScaleY != 1.0f;

    const auto drawOriginal = [&]()
    {
        ctx.r3 = r3;
        ctx.r4 = ctx.r1;
        ctx.r5 = r5;

        if (!needsTargetScale)
        {
            original(ctx, base);
            return;
        }

        const auto scaledSize = (size + 0x0F) & ~0x0F;

        ctx.r1.u32 -= scaledSize;
        ctx.r4 = ctx.r1;

        const auto scaled = base + ctx.r1.u32;

        memcpy(scaled, stack, size);

        for (size_t i = 0; i < r5.u32; i++)
        {
            const auto vertex = reinterpret_cast<Chao::CSD::Vertex*>(scaled + (i * stride));

            vertex->X = vertex->X * targetScaleX;
            vertex->Y = vertex->Y * targetScaleY;
        }

        original(ctx, base);

        ctx.r1.u32 += scaledSize;
    };

    const auto flipHorizontally = [&]()
    {
        const auto tlColor = getVertex(tl)->Color;
        const auto blColor = getVertex(bl)->Color;
        const auto trColor = getVertex(tr)->Color;
        const auto brColor = getVertex(br)->Color;
        const auto tlU = getVertex(tl)->U;
        const auto blU = getVertex(bl)->U;
        const auto trU = getVertex(tr)->U;
        const auto brU = getVertex(br)->U;

        getVertex(tl)->Color = trColor;
        getVertex(bl)->Color = brColor;
        getVertex(tr)->Color = tlColor;
        getVertex(br)->Color = blColor;
        getVertex(tl)->U = trU;
        getVertex(bl)->U = brU;
        getVertex(tr)->U = tlU;
        getVertex(br)->U = blU;
    };

    const auto flipVertically = [&]()
    {
        const auto tlColor = getVertex(tl)->Color;
        const auto blColor = getVertex(bl)->Color;
        const auto trColor = getVertex(tr)->Color;
        const auto brColor = getVertex(br)->Color;
        const auto tlV = getVertex(tl)->V;
        const auto blV = getVertex(bl)->V;
        const auto trV = getVertex(tr)->V;
        const auto brV = getVertex(br)->V;

        getVertex(tl)->Color = blColor;
        getVertex(bl)->Color = tlColor;
        getVertex(tr)->Color = brColor;
        getVertex(br)->Color = trColor;
        getVertex(tl)->V = blV;
        getVertex(bl)->V = tlV;
        getVertex(tr)->V = brV;
        getVertex(br)->V = trV;
    };

    const auto applyPositionModifier = [&](CsdVertexModifierGroup* in_pModifier)
    {
        getVertex(tl)->X = getVertex(tl)->X + in_pModifier->GetVertexModifier(tl).X + in_pModifier->Offset.X;
        getVertex(tl)->Y = getVertex(tl)->Y + in_pModifier->GetVertexModifier(tl).Y + in_pModifier->Offset.Y;
        getVertex(bl)->X = getVertex(bl)->X + in_pModifier->GetVertexModifier(bl).X + in_pModifier->Offset.X;
        getVertex(bl)->Y = getVertex(bl)->Y + in_pModifier->GetVertexModifier(bl).Y + in_pModifier->Offset.Y;
        getVertex(tr)->X = getVertex(tr)->X + in_pModifier->GetVertexModifier(tr).X + in_pModifier->Offset.X;
        getVertex(tr)->Y = getVertex(tr)->Y + in_pModifier->GetVertexModifier(tr).Y + in_pModifier->Offset.Y;
        getVertex(br)->X = getVertex(br)->X + in_pModifier->GetVertexModifier(br).X + in_pModifier->Offset.X;
        getVertex(br)->Y = getVertex(br)->Y + in_pModifier->GetVertexModifier(br).Y + in_pModifier->Offset.Y;
    };

    const auto applyUVModifier = [&](CsdVertexModifierGroup* in_pModifier)
    {
        getVertex(tl)->U = getVertex(tl)->U + in_pModifier->GetVertexModifier(tl).U;
        getVertex(tl)->V = getVertex(tl)->V + in_pModifier->GetVertexModifier(tl).V;
        getVertex(bl)->U = getVertex(bl)->U + in_pModifier->GetVertexModifier(bl).U;
        getVertex(bl)->V = getVertex(bl)->V + in_pModifier->GetVertexModifier(bl).V;
        getVertex(tr)->U = getVertex(tr)->U + in_pModifier->GetVertexModifier(tr).U;
        getVertex(tr)->V = getVertex(tr)->V + in_pModifier->GetVertexModifier(tr).V;
        getVertex(br)->U = getVertex(br)->U + in_pModifier->GetVertexModifier(br).U;
        getVertex(br)->V = getVertex(br)->V + in_pModifier->GetVertexModifier(br).V;
    };

    const auto applyColorModifier = [&](CsdVertexModifierGroup* in_pModifier)
    {
        in_pModifier->GetVertexModifier(tl).ApplyColorModifier(getVertex(tl));
        in_pModifier->GetVertexModifier(bl).ApplyColorModifier(getVertex(bl));
        in_pModifier->GetVertexModifier(tr).ApplyColorModifier(getVertex(tr));
        in_pModifier->GetVertexModifier(br).ApplyColorModifier(getVertex(br));
    };

    const auto applyRepeatModifiers = [&]()
    {
        if ((flags & CSD_REPEAT_FLIP_HORIZONTAL) != 0)
            flipHorizontally();

        if ((flags & CSD_REPEAT_FLIP_VERTICAL) != 0)
            flipVertically();

        if ((flags & CSD_REPEAT_POSITION_MODIFIER) != 0)
            applyPositionModifier(repeatVertexModifiers);

        if ((flags & CSD_REPEAT_UV_MODIFIER) != 0)
            applyUVModifier(repeatVertexModifiers);
            
        if ((flags & CSD_REPEAT_COLOR_MODIFIER) != 0)
            applyColorModifier(repeatVertexModifiers);
    };

    if ((flags & CSD_POSITION_MODIFIER) != 0)
        applyPositionModifier(baseVertexModifiers);

    if ((flags & CSD_UV_MODIFIER) != 0)
        applyUVModifier(baseVertexModifiers);

    if ((flags & CSD_COLOR_MODIFIER) != 0)
        applyColorModifier(baseVertexModifiers);

    if ((flags & CSD_CROP_TO_WIDESCREEN) != 0)
    {
        const auto v0 = getVertex(tl);
        const auto v2 = getVertex(tr);

        const float left = BlackBar::s_pillarboxWidth;
        const float right = vpWidth - BlackBar::s_pillarboxWidth;
        const float uvWidth = v2->U - v0->U;

        const auto cropLeft = [&](Chao::CSD::Vertex* in_pVertex)
        {
            if (in_pVertex->X < left)
            {
                in_pVertex->U = in_pVertex->U + ((left - in_pVertex->X) / width) * uvWidth;
                in_pVertex->X = left;
            }
        };

        const auto cropRight = [&](Chao::CSD::Vertex* in_pVertex)
        {
            if (in_pVertex->X > right)
            {
                in_pVertex->U = in_pVertex->U - ((in_pVertex->X - right) / width) * uvWidth;
                in_pVertex->X = right;
            }
        };

        cropLeft(v0);
        cropLeft(getVertex(bl));
        cropRight(v2);
        cropRight(getVertex(br));
    }

    if ((flags & CSD_REPEAT_LEFT) != 0)
    {
        auto x = getVertex(tr)->X;

        auto arrowIndex = 0;
        auto arrowCount = 0;
        const auto arrowWidth = (width - (width / 2.5f));

        if ((flags & CSD_CHEVRON) != 0)
        {
            // Shift root arrow forwards to use entirely custom arrows.
            for (size_t i = 0; i < r5.u32; i++)
                getVertex(i)->X = getVertex(i)->X + arrowWidth;

            // Compute number of foreground arrows.
            while (x > 0.0f)
            {
                x = x - arrowWidth;
                arrowCount++;
            }

            g_fgArrowsEnd = (CHEVRON_INTRO_DURATION / 2.0f) * float(arrowCount + 1);

            // Reset X to first vertex.
            x = getVertex(0)->X;
        }

        bool hasExtendedLeft{};

        while (x > 0.0f)
        {
            drawOriginal();

            if (hasExtendedLeft)
                break;

            if ((flags & CSD_CHEVRON) != 0)
            {
                ///////////////////////////////////////////////////////////
                // Clone foreground arrows to recreate chevron animation //
                ///////////////////////////////////////////////////////////

                // These values are based off the animation in background.xncp.
                auto endAlpha = uint8_t(std::lerp(30, 10, x / vpWidth));
                auto introDuration = CHEVRON_INTRO_DURATION;
                auto introStartTime = (introDuration / 2.0f) * float(arrowIndex + 1) + g_bgArrowsEnd;
                auto outroDuration = CHEVRON_OUTRO_DURATION;
                auto outroStartTime = (g_fgArrowsEnd + introStartTime) + 0.5f;

                // This will be set by the last foreground arrow in the
                // sequence, therefore making it the loop end point.
                g_chevronLoopTime = outroStartTime + outroDuration;

                // Store current arrow animation progress.
                auto& arrow = g_fgArrows[arrowIndex];
                arrow.EndAlpha = endAlpha;
                arrow.IntroDuration = introDuration;
                arrow.IntroStartTime = introStartTime;
                arrow.OutroDuration = outroDuration;
                arrow.OutroStartTime = outroStartTime;

                // Animate arrow position and alpha.
                for (size_t i = 0; i < r5.u32; i++)
                {
                    getVertex(i)->X = (getVertex(i)->X - arrowWidth) - g_bgArrows[arrowIndex].CurrentOffsetX;
                    getVertex(i)->Color.A(g_fgArrows[arrowIndex].CurrentAlpha);
                }

                arrowIndex++;
            }
            else
            {
                // Move cloned cast to left edge of existing cast.
                for (size_t i = 0; i < r5.u32; i++)
                    getVertex(i)->X = getVertex(i)->X - (width + repeatVertexModifiers->Offset.X);
            }

            // Update loop condition.
            x = getVertex(tr)->X;

            applyRepeatModifiers();

            // Extend to the left of the screen.
            if ((flags & CSD_REPEAT_EXTEND) != 0)
            {
                for (size_t i = 0; i < r5.u32; i++)
                {
                    if (i == tl || i == bl)
                        getVertex(i)->X = std::lerp(vpWidth, std::min(getVertex(i)->X.get(), 0.0f), repeatVertexModifiers->ExtendBias.X);
                }

                hasExtendedLeft = true;
            }
        }
    }

    if ((flags & CSD_REPEAT_RIGHT) != 0)
    {
        auto x = getVertex(tl)->X;

        auto arrowIndex = 0;
        auto arrowCount = 0;
        const auto arrowWidth = (width - (width / 4.0f));

        if ((flags & CSD_CHEVRON) != 0)
        {
            // Shift root arrow backwards to use entirely custom arrows.
            for (size_t i = 0; i < r5.u32; i++)
                getVertex(i)->X = getVertex(i)->X - arrowWidth;

            // Compute number of background arrows.
            while (x < vpWidth)
            {
                x = x + arrowWidth;
                arrowCount++;
            }

            g_bgArrowsEnd = (CHEVRON_INTRO_DURATION / 2.0f) * float(arrowCount + 1);

            // Reset X to first vertex.
            x = getVertex(0)->X;
        }

        bool hasExtendedRight{};

        while (x < vpWidth)
        {
            drawOriginal();

            if (hasExtendedRight)
                break;

            if ((flags & CSD_CHEVRON) != 0)
            {
                ///////////////////////////////////////////////////////////
                // Clone background arrows to recreate chevron animation //
                ///////////////////////////////////////////////////////////

                // These values are based off the animation in background.xncp.
                auto endAlpha = uint8_t(std::lerp(10, 35, x / vpWidth));
                auto introDuration = CHEVRON_INTRO_DURATION;
                auto introStartTime = (introDuration / 2.0f) * float(arrowIndex + 1);
                auto outroDuration = CHEVRON_OUTRO_DURATION;
                auto outroStartTime = (g_bgArrowsEnd + introStartTime) + 0.5f;

                // Store current arrow animation progress.
                auto& arrow = g_bgArrows[arrowIndex];
                arrow.EndAlpha = endAlpha;
                arrow.IntroDuration = introDuration;
                arrow.IntroStartTime = introStartTime;
                arrow.OutroDuration = outroDuration;
                arrow.OutroStartTime = outroStartTime;

                // Animate arrow position and alpha.
                for (size_t i = 0; i < r5.u32; i++)
                {
                    getVertex(i)->X = (getVertex(i)->X + arrowWidth) + g_bgArrows[arrowIndex].CurrentOffsetX;
                    getVertex(i)->Color.A(g_bgArrows[arrowIndex].CurrentAlpha);
                }

                arrowIndex++;
            }
            else
            {
                // Move cloned cast to right edge of existing cast.
                for (size_t i = 0; i < r5.u32; i++)
                    getVertex(i)->X = getVertex(i)->X + (width + repeatVertexModifiers->Offset.X);
            }

            // Update loop condition.
            x = getVertex(tl)->X;

            applyRepeatModifiers();

            // Extend to the right of the screen.
            if ((flags & CSD_REPEAT_EXTEND) != 0)
            {
                for (size_t i = 0; i < r5.u32; i++)
                {
                    if (i == tr || i == br)
                        getVertex(i)->X = std::lerp(0.0f, std::max(getVertex(i)->X.get(), vpWidth), repeatVertexModifiers->ExtendBias.X);
                }

                hasExtendedRight = true;
            }
        }
    }

    if ((flags & CSD_REPEAT_UP) != 0)
    {
        auto y = getVertex(bl)->Y;

        bool hasExtendedUp{};

        while (y > 0.0f)
        {
            drawOriginal();

            if (hasExtendedUp)
                break;

            // Move cloned cast to top edge of existing cast.
            for (size_t i = 0; i < r5.u32; i++)
                getVertex(i)->Y = getVertex(i)->Y - (height + repeatVertexModifiers->Offset.Y);

            // Update loop condition.
            y = getVertex(bl)->Y;

            applyRepeatModifiers();

            // Extend to the top of the screen.
            if ((flags & CSD_REPEAT_EXTEND) != 0)
            {
                for (size_t i = 0; i < r5.u32; i++)
                {
                    if (i == tl || i == tr)
                        getVertex(i)->Y = std::lerp(vpHeight, std::min(getVertex(i)->Y.get(), 0.0f), repeatVertexModifiers->ExtendBias.Y);
                }

                hasExtendedUp = true;
            }
        }
    }

    if ((flags & CSD_REPEAT_DOWN) != 0)
    {
        auto y = getVertex(tl)->Y;

        bool hasExtendedDown{};

        while (y < vpHeight)
        {
            drawOriginal();

            if (hasExtendedDown)
                break;

            // Move cloned cast to bottom edge of existing cast.
            for (size_t i = 0; i < r5.u32; i++)
                getVertex(i)->Y = getVertex(i)->Y + (height + repeatVertexModifiers->Offset.Y);

            // Update loop condition.
            y = getVertex(tl)->Y;

            applyRepeatModifiers();

            // Extend to the bottom of the screen.
            if ((flags & CSD_REPEAT_EXTEND) != 0)
            {
                for (size_t i = 0; i < r5.u32; i++)
                {
                    if (i == bl || i == br)
                        getVertex(i)->Y = std::lerp(0.0f, std::max(getVertex(i)->Y.get(), vpHeight), repeatVertexModifiers->ExtendBias.Y);
                }

                hasExtendedDown = true;
            }
        }
    }
    
    if ((flags & (CSD_REPEAT_LEFT | CSD_REPEAT_RIGHT | CSD_REPEAT_UP | CSD_REPEAT_DOWN)) == 0)
    {
        drawOriginal();

        // Clone Audio Room "pod" left edge to fill
        // in the rest of the box at ultrawide.
        if ((flags & CSD_POD_CLONE) != 0 && g_aspectRatio > WIDE_ASPECT_RATIO)
        {
            const auto v0 = getVertex(CSD_TL);
            const auto v1 = getVertex(CSD_BL);
            const auto v2 = getVertex(CSD_TR);
            const auto v3 = getVertex(CSD_BR);

            // Shift cloned element to base edge.
            v0->X = g_podBaseRightX;
            v1->X = g_podBaseRightX;

            // Stretch cloned element to screen edge.
            v2->X = vpWidth;
            v3->X = vpWidth;

            // Shift UVs to remove cloned element edge line.
            v0->U = v0->U + 0.008f;
            v0->V = v0->V + 0.008f;
            v1->U = v1->U + 0.008f;
            v1->V = v1->V + 0.008f;

            drawOriginal();
        }

        // Clone CRI logo to move the technology
        // logos to the true bottom right corner.
        if ((flags & CSD_CRI_LOGO) != 0)
        {
            auto v0 = getVertex(CSD_TL);
            auto v1 = getVertex(CSD_BL);
            auto v2 = getVertex(CSD_TR);
            auto v3 = getVertex(CSD_BR);

            const auto originalTop = v0->Y;
            const auto originalHeight = v1->Y - v0->Y;

            const auto width = 422.0f;
            const auto height = 132.0f;
            const auto widthScaled = width * g_aspectRatioScale;
            const auto heightScaled = height * g_aspectRatioScale;

            /////////////////////////////////////////////////////////////////////////
            // Clone and scale white square to block the original technology logos //
            /////////////////////////////////////////////////////////////////////////

            const auto top = originalTop + (originalHeight - heightScaled);
            const auto left = g_aspectRatio >= WIDE_ASPECT_RATIO ? 0.0f : vpWidth - widthScaled;

            // Top Left
            v0->X = left;
            v0->Y = top;
            v0->U = 0.0f;
            v0->V = 0.01f;

            // Bottom Left
            v1->X = left;
            v1->Y = vpHeight;
            v1->U = 0.0f;
            v1->V = 0.01f;

            // Top Right
            v2->X = vpWidth;
            v2->Y = top;
            v2->U = 0.0f;
            v2->V = 0.01f;

            // Bottom Right
            v3->X = vpWidth;
            v3->Y = vpHeight;
            v3->U = 0.0f;
            v3->V = 0.01f;

            drawOriginal();

            /////////////////////////////////////////////
            // Clone and scale to fit technology logos //
            /////////////////////////////////////////////

            v0 = getVertex(0);
            v1 = getVertex(1);
            v2 = getVertex(2);
            v3 = getVertex(3);

            const auto [min, max] = PIXELS_TO_UV_COORDS(2048, 512, 858, 380, width, height);

            // Top Left
            v0->X = vpWidth - widthScaled;
            v0->Y = vpHeight - heightScaled;
            v0->U = min.x;
            v0->V = min.y;

            // Bottom Left
            v1->X = vpWidth - widthScaled;
            v1->Y = vpHeight;
            v1->U = min.x;
            v1->V = max.y;

            // Top Right
            v2->X = vpWidth;
            v2->Y = vpHeight - heightScaled;
            v2->U = max.x;
            v2->V = min.y;

            // Bottom Right
            v3->X = vpWidth;
            v3->Y = vpHeight;
            v3->U = max.x;
            v3->V = max.y;

            drawOriginal();
        }
    }

    ctx.r1.u32 += (size + 0x0F) & ~0x0F;
}

// Sonicteam::CPlatformMarathon::Draw
PPC_FUNC_IMPL(__imp__sub_826315C8);
PPC_FUNC(sub_826315C8)
{
    // r3 = Sonicteam::CPlatformMarathon*
    // r4 = Vertex[r5]
    // r5 = Vertex Count

    Draw(ctx, base, __imp__sub_826315C8, 0x14);
}

// Sonicteam::CPlatformMarathon::DrawNoTex
PPC_FUNC_IMPL(__imp__sub_82631718);
PPC_FUNC(sub_82631718)
{
    // r3 = Sonicteam::CPlatformMarathon*
    // r4 = Vertex[r5]
    // r5 = Vertex Count

    Draw(ctx, base, __imp__sub_82631718, 0x0C);
}

// Set CSD scale.
PPC_FUNC_IMPL(__imp__sub_828C78E0);
PPC_FUNC(sub_828C78E0)
{
    ctx.f1.f64 = 1280.0;
    ctx.f2.f64 = 720.0;

    __imp__sub_828C78E0(ctx, base);
}

// Sonicteam::EventEntityTask::Update
PPC_FUNC_IMPL(__imp__sub_8264AC48);
PPC_FUNC(sub_8264AC48)
{
    if (Config::CutsceneAspectRatio == ECutsceneAspectRatio::Original)
        BlackBar::Show();

    __imp__sub_8264AC48(ctx, base);
}

// Sonicteam::EndingMode::Update
PPC_FUNC_IMPL(__imp__sub_824A4A40);
PPC_FUNC(sub_824A4A40)
{
    BlackBar::Show();

    __imp__sub_824A4A40(ctx, base);
}

// Sonicteam::HUDLimitTime::ProcessMessage
PPC_FUNC_IMPL(__imp__sub_824D6E50);
PPC_FUNC(sub_824D6E50)
{
    if (g_aspectRatio < WIDE_ASPECT_RATIO && Config::UIAlignmentMode == EUIAlignmentMode::Centre)
    {
        __imp__sub_824D6E50(ctx, base);
        return;
    }

    auto pHUDLimitTime = (Sonicteam::HUDLimitTime*)(base + ctx.r3.u32);

    pHUDLimitTime->m_Y = 64.0f - (g_aspectRatioOffsetY / g_aspectRatioScale);

    GuestToHostFunction<int>(sub_824D6D38, pHUDLimitTime, (double)pHUDLimitTime->m_X, (double)pHUDLimitTime->m_Y);

    __imp__sub_824D6E50(ctx, base);
}

// Sonicteam::HUDRaderMap::Update
PPC_FUNC_IMPL(__imp__sub_824F1538);
PPC_FUNC(sub_824F1538)
{
    const auto pHUDRaderMap = static_cast<Sonicteam::HUDRaderMap*>(reinterpret_cast<Sonicteam::SoX::Engine::Task*>(base + ctx.r3.u32));

    __imp__sub_824F1538(ctx, base);

    pHUDRaderMap->m_pMaskTexture->m_Width = g_radarMapScale;
    pHUDRaderMap->m_pMaskTexture->m_Height = g_radarMapScale;
    pHUDRaderMap->m_X = g_radarMapX - g_radarMapCoverWidth / 2;
    pHUDRaderMap->m_Y = g_radarMapY - g_radarMapCoverHeight / 2;
}

// Sonicteam::CObjBalloonIconDrawable::Draw
PPC_FUNC_IMPL(__imp__sub_82352220);
PPC_FUNC(sub_82352220)
{
    const auto pCObjBalloonIconDrawable = reinterpret_cast<Sonicteam::CObjBalloonIconDrawable*>(base + ctx.r3.u32);

    auto scaleX = 1.0f * g_aspectRatioGameplayScale;
    auto scaleY = g_aspectRatio * g_aspectRatioGameplayScale;

    if (g_aspectRatio > WIDE_ASPECT_RATIO)
    {
        const auto ratio = g_aspectRatio / WIDE_ASPECT_RATIO;

        scaleX /= ratio;
        scaleY /= ratio;
    }

    pCObjBalloonIconDrawable->m_aVertices[0].X = -scaleX;
    pCObjBalloonIconDrawable->m_aVertices[0].Y = 0.0f;

    pCObjBalloonIconDrawable->m_aVertices[1].X = -scaleX;
    pCObjBalloonIconDrawable->m_aVertices[1].Y = scaleY;

    pCObjBalloonIconDrawable->m_aVertices[2].X = scaleX;
    pCObjBalloonIconDrawable->m_aVertices[2].Y = 0.0f;

    pCObjBalloonIconDrawable->m_aVertices[3].X = scaleX;
    pCObjBalloonIconDrawable->m_aVertices[3].Y = scaleY;

    __imp__sub_82352220(ctx, base);
}

// Sonicteam::MovieObject::~MovieObject
PPC_FUNC_IMPL(__imp__sub_8264D3C8);
PPC_FUNC(sub_8264D3C8)
{
    g_isMoviePlaying = false;

    __imp__sub_8264D3C8(ctx, base);
}

// Sonicteam::MovieObjectWmv::Draw
PPC_FUNC_IMPL(__imp__sub_8264CC90);
PPC_FUNC(sub_8264CC90)
{
    const auto pMovieObjectWmv = reinterpret_cast<Sonicteam::MovieObjectWmv*>(base + ctx.r3.u32);

    g_isMoviePlaying = true;

    static XXH64_hash_t s_movieNameHash{};
    static bool s_movieUsesCustomDimensions{};
    static float s_movieLeft{};
    static float s_movieRight{};
    static float s_movieTop{};
    static float s_movieBottom{};

    auto movieNameHash = HashStr(pMovieObjectWmv->m_FilePath.c_str());

    if (movieNameHash != s_movieNameHash)
    {
        s_movieNameHash = movieNameHash;
        s_movieUsesCustomDimensions = pMovieObjectWmv->m_UseCustomDimensions;
        s_movieLeft = pMovieObjectWmv->m_Left;
        s_movieRight = pMovieObjectWmv->m_Right;
        s_movieTop = pMovieObjectWmv->m_Top;
        s_movieBottom = pMovieObjectWmv->m_Bottom;

        LOGFN_UTILITY("Movie: {} - {}x{}", pMovieObjectWmv->m_FilePath.c_str(), pMovieObjectWmv->m_Width.get(), pMovieObjectWmv->m_Height.get());
    }

    const auto aspectRatioMovie = float(pMovieObjectWmv->m_Width) / float(pMovieObjectWmv->m_Height);

    float width{}, height{}, left{}, right{}, top{}, bottom{};

    MovieModifier movieModifier{};

    if (const auto pMovieModifier = FindHash(g_movieModifiers, movieNameHash))
        movieModifier = *pMovieModifier;

    if ((movieModifier.Flags & MOVIE_BGM) != 0)
    {
        if (const auto pAudioEngine = Sonicteam::AudioEngineXenon::GetInstance())
            pMovieObjectWmv->SetVolume(pAudioEngine->m_MusicVolume);
    }

    if (s_movieUsesCustomDimensions)
    {
        auto movieRectLeft = s_movieLeft;
        auto movieRectRight = s_movieRight;
        auto movieRectTop = s_movieTop;
        auto movieRectBottom = s_movieBottom;

        if (g_aspectRatio > aspectRatioMovie)
        {
            // Scale width at wide aspect ratios.
            movieRectLeft = s_movieLeft / (g_aspectRatio / aspectRatioMovie);
            movieRectRight = s_movieRight / (g_aspectRatio / aspectRatioMovie);
        }
        else
        {
            // Scale height at narrow aspect ratios.
            movieRectTop = s_movieTop / (aspectRatioMovie / g_aspectRatio);
            movieRectBottom = s_movieBottom / (aspectRatioMovie / g_aspectRatio);
        }

        auto movieRectCentreX = (movieRectLeft + movieRectRight) / 2.0f;
        auto movieRectCentreY = (movieRectTop + movieRectBottom) / 2.0f;

        width = movieRectRight - movieRectLeft;
        height = movieRectTop - movieRectBottom;

        left = movieRectCentreX - (width / 2.0f);
        right = movieRectCentreX + (width / 2.0f);
        top = movieRectCentreY + (height / 2.0f);
        bottom = movieRectCentreY - (height / 2.0f);
    }
    else
    {
        width = 2.0f;
        height = 2.0f;

        if (g_aspectRatio > aspectRatioMovie)
        {
            if ((movieModifier.Flags & MOVIE_CROP_WIDE) != 0)
            {
                // Crop vertically at wide aspect ratios.
                height = 2.0f * (g_aspectRatio / aspectRatioMovie);
            }
            else
            {
                // Pillarbox at wide aspect ratios.
                width = 2.0f * (aspectRatioMovie / g_aspectRatio);
            }
        }
        else
        {
            if ((movieModifier.Flags & MOVIE_CROP_NARROW) != 0)
            {
                // Crop horizontally at narrow aspect ratios.
                width = 2.0f * (aspectRatioMovie / g_aspectRatio);
            }
            else
            {
                // Letterbox at narrow aspect ratios.
                height = 2.0f * (g_aspectRatio / aspectRatioMovie);
            }
        }

        left = -width / 2.0f;
        right = (width / 2.0f) * 2.0f;
        top = height / 2.0f;
        bottom = (-height / 2.0f) * 2.0f;
    }

    pMovieObjectWmv->m_UseCustomDimensions = true;
    pMovieObjectWmv->m_Left = left;
    pMovieObjectWmv->m_Right = right;
    pMovieObjectWmv->m_Top = top;
    pMovieObjectWmv->m_Bottom = bottom;

    __imp__sub_8264CC90(ctx, base);
}

void ReplaceTextVariables(Sonicteam::TextEntity* pTextEntity)
{
    if (!pTextEntity || !pTextEntity->m_VariableVertexCount)
        return;

    const auto& pftModifier = Config::GetPreferredControllerIcons() == EControllerIcons::PlayStation
        ? g_buttonCropsPS3
        : g_buttonCropsXenon;

    const auto variables = Sonicteam::TextCard::MapVariables(pTextEntity->m_pVariables);
    const auto relativeVariableCount = pTextEntity->GetRelativeVariableCount(pTextEntity->m_PageIndex);

    int pageVariableIndex{};
    int pagePictureCount{};

    for (int i = 0; i < pTextEntity->m_Page.size(); i++)
    {
        const auto c = ByteSwap(pTextEntity->m_Page.c_str()[i]);

        if (c != u'$')
            continue;

        const auto globalVariableIndex = relativeVariableCount + pageVariableIndex;

        // Some strings may have more placeholder
        // characters than variables in the map.
        if (globalVariableIndex >= variables.size())
            break;

        const auto& variable = variables[globalVariableIndex];

        if (variable.first == "picture")
        {
            const auto hash = HashStr(variable.second);
            const auto pBaseParams = FindHash(g_buttonCropsXenon, hash);
            const auto pNewParams = FindHash(pftModifier, hash);

            if (pBaseParams && pNewParams)
            {
                const auto vi = pagePictureCount * 6;

                auto& v0 = pTextEntity->m_pVariableVertices[vi + 0];
                auto& v1 = pTextEntity->m_pVariableVertices[vi + 1];
                auto& v2 = pTextEntity->m_pVariableVertices[vi + 2];
                auto& v3 = pTextEntity->m_pVariableVertices[vi + 3];
                auto& v4 = pTextEntity->m_pVariableVertices[vi + 4];
                auto& v5 = pTextEntity->m_pVariableVertices[vi + 5];

                const auto centreX = (v0.X + v2.X) / 2.0f;
                const auto widthAdjust = float(pNewParams->Width) / float(pBaseParams->Width);

                const auto adjust = [&](auto& in_rVertex)
                {
                    in_rVertex.X = centreX + (in_rVertex.X - centreX) * widthAdjust;
                };

                const auto& [rMin, rMax] = PIXELS_TO_UV_COORDS(g_fontPictureWidth, g_fontPictureHeight, pNewParams->X, pNewParams->Y, pNewParams->Width, pNewParams->Height);

                // Bottom Left (Triangle 1)
                adjust(v0);
                v0.U = rMin.x;
                v0.V = rMin.y;

                // Top Left (Triangle 1)
                adjust(v1);
                v1.U = rMin.x;
                v1.V = rMax.y;

                // Top Right (Triangle 1)
                adjust(v2);
                v2.U = rMax.x;
                v2.V = rMax.y;

                // Bottom Left (Triangle 2)
                adjust(v3);
                v3.U = rMin.x;
                v3.V = rMin.y;

                // Top Right (Triangle 2)
                adjust(v4);
                v4.U = rMax.x;
                v4.V = rMax.y;

                // Bottom Right (Triangle 2)
                adjust(v5);
                v5.U = rMax.x;
                v5.V = rMin.y;

                pagePictureCount++;
            }
        }

        pageVariableIndex++;
    }
}

void SetTextEntityModifier(Sonicteam::TextEntity* pTextEntity, uint64_t flags)
{
    if (!pTextEntity)
        return;

    const auto pTextModifier = reinterpret_cast<uint64_t*>(
        reinterpret_cast<uint8_t*>(pTextEntity) + sizeof(Sonicteam::TextEntity));

    *pTextModifier = flags;

    pTextEntity->m_FieldDD = true;
    pTextEntity->Update();
}

class TextEntityEx : public Sonicteam::TextEntity
{
public:
    uint64_t m_Modifier;
};

void TextEntityAlloc(PPCRegister& r3)
{
    r3.u32 = sizeof(TextEntityEx);
}

// Sonicteam::TextEntity::TextEntity
PPC_FUNC_IMPL(__imp__sub_8262DC60);
PPC_FUNC(sub_8262DC60)
{
    const auto pTextEntity = reinterpret_cast<TextEntityEx*>(base + ctx.r3.u32);

    pTextEntity->m_Modifier = CSD_ALIGN_CENTER;

    __imp__sub_8262DC60(ctx, base);
}

// Sonicteam::TextEntity::Draw
PPC_FUNC_IMPL(__imp__sub_8262D868);
PPC_FUNC(sub_8262D868)
{
    const auto pTextEntity = reinterpret_cast<TextEntityEx*>(base + ctx.r3.u32);

    auto textModifier = pTextEntity->m_Modifier;

    if (!App::s_isDrawUI || (textModifier & CSD_SKIP) != 0 ||
        ((textModifier & CSD_LIFE_COUNTER) != 0 && Config::DisableLives))
    {
        for (size_t i = 0; i < pTextEntity->m_TextVertexCount; i++)
            pTextEntity->m_pTextVertices[i].Color.A(0);

        for (size_t i = 0; i < pTextEntity->m_VariableVertexCount; i++)
            pTextEntity->m_pVariableVertices[i].Color.A(0);

        return;
    }

    auto x = pTextEntity->m_X;
    auto y = pTextEntity->m_Y;
    auto scaleX = pTextEntity->m_ScaleX;
    auto scaleY = pTextEntity->m_ScaleY;

    auto offsetX = 0.0f;
    auto offsetY = 0.0f;
    auto scale = g_aspectRatioScale;

    if (Config::UIAlignmentMode == EUIAlignmentMode::Centre || BlackBar::IsVisible())
    {
        if (g_aspectRatio > WIDE_ASPECT_RATIO)
        {
            textModifier |= CSD_ALIGN_IGNORE_PREFERENCE_H;
        }
        else if (g_aspectRatio < WIDE_ASPECT_RATIO)
        {
            textModifier |= CSD_ALIGN_IGNORE_PREFERENCE_V;
        }
    }

    // Ensure subtitle text appears inside movie viewport.
    if ((textModifier & CSD_EVENT) != 0)
    {
        if (g_isMoviePlaying)
        {
            textModifier |= CSD_ALIGN_IGNORE_PREFERENCE_V;
        }
        else
        {
            textModifier &= ~CSD_ALIGN_IGNORE_PREFERENCE_V;
        }
    }

    if ((textModifier & CSD_ALIGN_IGNORE_PREFERENCE_H) != 0)
        textModifier &= ~(CSD_ALIGN_LEFT | CSD_ALIGN_RIGHT);

    if ((textModifier & CSD_ALIGN_IGNORE_PREFERENCE_V) != 0)
        textModifier &= ~(CSD_ALIGN_TOP | CSD_ALIGN_BOTTOM);

    if ((textModifier & CSD_ALIGN_RIGHT) != 0)
        offsetX = g_aspectRatioOffsetX * 2.0f;
    else if ((textModifier & CSD_ALIGN_LEFT) == 0)
        offsetX = g_aspectRatioOffsetX;

    if ((textModifier & CSD_ALIGN_BOTTOM) != 0)
        offsetY = g_aspectRatioOffsetY * 2.0f;
    else if ((textModifier & CSD_ALIGN_TOP) == 0)
        offsetY = g_aspectRatioOffsetY;

    const auto fitScale = std::min((Video::s_viewportWidth / 1280.0f) / g_aspectRatioScale, 1.0f);

    scale *= fitScale;

    if ((textModifier & CSD_ALIGN_RIGHT) != 0)
        offsetX += 1280.0f * (1.0f - fitScale) * g_aspectRatioScale;
    else if ((textModifier & CSD_ALIGN_LEFT) == 0)
        offsetX += g_aspectRatioOffsetX + 640.0f * (1.0f - fitScale) * g_aspectRatioScale - g_aspectRatioOffsetX;

    if ((textModifier & CSD_ALIGN_BOTTOM) != 0)
        offsetY += 720.0f * (1.0f - fitScale) * g_aspectRatioScale;
    else if ((textModifier & CSD_ALIGN_TOP) == 0)
        offsetY += g_aspectRatioOffsetY + 360.0f * (1.0f - fitScale) * g_aspectRatioScale - g_aspectRatioOffsetY;

    pTextEntity->m_X = offsetX + pTextEntity->m_X * scale;
    pTextEntity->m_Y = offsetY + pTextEntity->m_Y * scale;
    pTextEntity->m_ScaleX = scale;
    pTextEntity->m_ScaleY = scale;

    __imp__sub_8262D868(ctx, base);

    pTextEntity->m_X = x;
    pTextEntity->m_Y = y;
    pTextEntity->m_ScaleX = scaleX;
    pTextEntity->m_ScaleY = scaleY;

    ReplaceTextVariables(pTextEntity);
}

// Sonicteam::HUDMainMenu::Destroy
PPC_FUNC_IMPL(__imp__sub_824E2978);
PPC_FUNC(sub_824E2978)
{
    g_bgArrows.clear();
    g_fgArrows.clear();

    __imp__sub_824E2978(ctx, base);
}

// Sonicteam::HUDMainMenu::Update
PPC_FUNC_IMPL(__imp__sub_824E11D0);
PPC_FUNC(sub_824E11D0)
{
    auto pHUDMainMenu = (Sonicteam::HUDMainMenu*)(base + ctx.r3.u32 - 8);
    auto pHudTextRoot = pHUDMainMenu->m_pHudTextRoot;

    static bool s_preservedTextParams{};
    static float s_multiplayerTextOffsetY{};
    static float s_tagTextOffsetY{};
    static float s_battleTextOffsetY{};

    static constexpr double HIDE_TEXT_OFFSET = -100000.0f;

    auto isTrialSelect = MainMenuTaskPatches::s_state >= 12 && MainMenuTaskPatches::s_state <= 15;

    auto isTag = MainMenuTaskPatches::s_state == Sonicteam::MainMenuTask::MainMenuState_Tag ||
                 MainMenuTaskPatches::s_state == Sonicteam::MainMenuTask::MainMenuState_Tag1PSelect ||
                 MainMenuTaskPatches::s_state == Sonicteam::MainMenuTask::MainMenuState_ExitToStage;

    while (pHudTextRoot)
    {
        if (pHudTextRoot->m_SceneName == "main_menu")
        {
            if (pHudTextRoot->m_CastName == "multi_player")
            {
                if (!s_preservedTextParams)
                    s_multiplayerTextOffsetY = pHudTextRoot->m_OffsetY;

                pHudTextRoot->m_OffsetY = isTrialSelect ? HIDE_TEXT_OFFSET : s_multiplayerTextOffsetY;
            }
            else if (pHudTextRoot->m_CastName == "tag")
            {
                if (!s_preservedTextParams)
                    s_tagTextOffsetY = pHudTextRoot->m_OffsetY;

                pHudTextRoot->m_OffsetY = (isTrialSelect || isTag) ? HIDE_TEXT_OFFSET : s_tagTextOffsetY;
            }
            else if (pHudTextRoot->m_CastName == "battle")
            {
                if (!s_preservedTextParams)
                    s_battleTextOffsetY = pHudTextRoot->m_OffsetY;

                pHudTextRoot->m_OffsetY = isTrialSelect ? HIDE_TEXT_OFFSET : s_battleTextOffsetY;
            }
        }

        pHudTextRoot = pHudTextRoot->m_pNext;
    }

    s_preservedTextParams = true;

    // Draw faded letterbox at narrow aspect ratios.
    if (!OptionsMenu::s_isVisible && g_aspectRatio < NARROW_ASPECT_RATIO_MAIN_MENU)
    {
        BlackBar::Show(true);
        BlackBar::SetBorderMargin(Scale(BlackBar::ms_MenuBorderMargin, true));
    }

    __imp__sub_824E11D0(ctx, base);
}

// Sonicteam::Spanverse::_acmSubtitle::ExecCallingNow
PPC_FUNC_IMPL(__imp__sub_82666490);
PPC_FUNC(sub_82666490)
{
    const auto pAcmSubtitle = reinterpret_cast<Sonicteam::Spanverse::_acmSubtitle*>(base + ctx.r3.u32);

    SetTextEntityModifier(pAcmSubtitle->m_spTextEntity.get(), CSD_ALIGN_BOTTOM | CSD_EVENT);

    __imp__sub_82666490(ctx, base);
}

// Sonicteam::HUDMainDisplay::Update
PPC_FUNC_IMPL(__imp__sub_824DCF40);
PPC_FUNC(sub_824DCF40)
{
    auto pHUDMainDisplay = (Sonicteam::HUDMainDisplay*)(base + ctx.r3.u32);

    SetTextEntityModifier(pHUDMainDisplay->m_Field184.get(), CSD_ALIGN_RIGHT);
    SetTextEntityModifier(pHUDMainDisplay->m_spTrickPointText.get(), CSD_ALIGN_RIGHT);
    SetTextEntityModifier(pHUDMainDisplay->m_Field194.get(), CSD_ALIGN_RIGHT);
    SetTextEntityModifier(pHUDMainDisplay->m_spSavePointTimeText.get(), CSD_ALIGN_BOTTOM);

    __imp__sub_824DCF40(ctx, base);
}

// Sonicteam::HintWindowTask::Update
PPC_FUNC_IMPL(__imp__sub_824D12F0);
PPC_FUNC(sub_824D12F0)
{
    auto pHintWindowTask = (Sonicteam::HintWindowTask*)(base + ctx.r3.u32);

    SetTextEntityModifier(pHintWindowTask->m_Field8C.get(), CSD_ALIGN_BOTTOM);
    SetTextEntityModifier(pHintWindowTask->m_Field94.get(), CSD_ALIGN_BOTTOM);

    __imp__sub_824D12F0(ctx, base);
}

// Sonicteam::MessageWindowTask::Update
PPC_FUNC_IMPL(__imp__sub_82507E68);
PPC_FUNC(sub_82507E68)
{
    auto pMessageWindowTask = (Sonicteam::MessageWindowTask*)(base + ctx.r3.u32);

    SetTextEntityModifier(pMessageWindowTask->m_Field90.get(), CSD_ALIGN_BOTTOM);
    SetTextEntityModifier(pMessageWindowTask->m_Field98.get(), CSD_ALIGN_BOTTOM);

    __imp__sub_82507E68(ctx, base);
}

// Sonicteam::TextFontPicture::LoadResource
PPC_FUNC_IMPL(__imp__sub_8263CC40);
PPC_FUNC(sub_8263CC40)
{
    auto pTextFontPicture = (Sonicteam::TextFontPicture*)(base + ctx.r3.u32);

    __imp__sub_8263CC40(ctx, base);

    g_fontPictureWidth = pTextFontPicture->m_TextureWidth;
    g_fontPictureHeight = pTextFontPicture->m_TextureHeight;
}

// Sonicteam::HUDGoldMedal::Update
PPC_FUNC_IMPL(__imp__sub_824D5C08);
PPC_FUNC(sub_824D5C08)
{
    auto pHUDGoldMedal = static_cast<Sonicteam::HUDGoldMedal*>(reinterpret_cast<Sonicteam::SoX::Engine::Task*>(base + ctx.r3.u32));

    for (auto& spTextEntity : pHUDGoldMedal->m_aspTextEntities)
    {
        uint64_t flags = CSD_ALIGN_CENTER;

        if (AchievementMenu::s_state == AchievementMenuState::Achievements)
            flags |= CSD_SKIP;

        SetTextEntityModifier(spTextEntity.get(), flags);
    }

    __imp__sub_824D5C08(ctx, base);
}

// Sonicteam::MainMenuExpositionTask::Update
PPC_FUNC_IMPL(__imp__sub_824FD868);
PPC_FUNC(sub_824FD868)
{
    auto pMainMenuExpositionTask = static_cast<Sonicteam::MainMenuExpositionTask*>(reinterpret_cast<Sonicteam::SoX::Engine::Task*>(base + ctx.r3.u32));

    // Default fade speed is 15 frames at 60 FPS.
    pMainMenuExpositionTask->m_FadeSpeed = (1.0f / 15.0f) * (60.0 * App::s_deltaTime);

    SetTextEntityModifier(pMainMenuExpositionTask->m_spDescriptionEntity.get(), CSD_ALIGN_CENTER);
    SetTextEntityModifier(pMainMenuExpositionTask->m_spPrevDescriptionEntity.get(), CSD_ALIGN_CENTER);

    __imp__sub_824FD868(ctx, base);
}

const xxHashMap<MovieModifier> g_movieModifiers =
{
    { HashStr("sound\\title_loop_GBn.wmv"), { MOVIE_CROP_NARROW | MOVIE_BGM } }
};
