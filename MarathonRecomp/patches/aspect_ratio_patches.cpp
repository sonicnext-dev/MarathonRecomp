#include "aspect_ratio_patches.h"
#include <api/Marathon.h>
#include <gpu/video.h>
#include <hid/hid.h>
#include <patches/hook_event.h>
#include <patches/loading_patches.h>
#include <patches/MainMenuTask_patches.h>
#include <ui/black_bar.h>
#include <ui/game_window.h>
#include <ui/imgui_utils.h>
#include <user/config.h>
#include <app.h>

// #define CORNER_DEBUG

constexpr float CHEVRON_INTRO_DURATION = 0.083f;
constexpr float CHEVRON_OUTRO_DURATION = 2.01666666666667f;

static Mutex g_pathMutex;
static std::map<const void*, XXH64_hash_t> g_paths{};

static std::optional<CsdModifier> g_sceneModifier{};
static std::optional<CsdModifier> g_castNodeModifier{};
static std::optional<CsdModifier> g_castModifier{};

static Sonicteam::TextFontPicture g_textFontPicture{};

static float g_corners[8]{};
static bool g_cornerExtract{};

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

static class LoadingPillarboxEvent : public HookEvent
{
public:
    void Update(float deltaTime) override
    {
        if (g_aspectRatio > WIDE_ASPECT_RATIO)
            BlackBar::g_isVisible = true;
    }
}
g_loadingPillarboxEvent{};

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
    return ((aspectRatio * 720.0f) / 1280.0f) / sqrt((aspectRatio * 720.0f) / 1280.0f);
}

void AspectRatioPatches::Init()
{
    LoadingPatches::Events.push_back(&g_loadingPillarboxEvent);
    MainMenuTaskPatches::Events.push_back(&g_chevronAnimResetEvent);
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

        // keep same scale above Steam Deck aspect ratio
        if (g_aspectRatio < WIDE_ASPECT_RATIO)
        {
            // interpolate to original 4:3 scale
            float steamDeckScale = g_aspectRatio / WIDE_ASPECT_RATIO;
            float narrowScale = ComputeScale(NARROW_ASPECT_RATIO);
            float lerpFactor = std::clamp((g_aspectRatio - NARROW_ASPECT_RATIO) / (STEAM_DECK_ASPECT_RATIO - NARROW_ASPECT_RATIO), 0.0f, 1.0f);

            g_aspectRatioGameplayScale = narrowScale + (steamDeckScale - narrowScale) * lerpFactor;
        }
    }
    else
    {
        // 4:3 crop
        g_aspectRatioOffsetX = (width - width * NARROW_ASPECT_RATIO) / 2.0f;
        g_aspectRatioOffsetY = (height - width / NARROW_ASPECT_RATIO) / 2.0f;
        g_aspectRatioScale = width / 960.0f;
        g_aspectRatioGameplayScale = ComputeScale(NARROW_ASPECT_RATIO);
    } 

    g_aspectRatioMultiplayerOffsetX = g_aspectRatioOffsetX / 2.0f;

    g_aspectRatioNarrowScale = std::clamp((g_aspectRatio - NARROW_ASPECT_RATIO) / (WIDE_ASPECT_RATIO - NARROW_ASPECT_RATIO), 0.0f, 1.0f);
    g_radarMapScale = 256 * g_aspectRatioScale * g_aspectRatioGameplayScale;
}

void EmplacePath(const void* key, const std::string_view& value)
{
    std::lock_guard lock(g_pathMutex);
    g_paths.emplace(key, HashStr(value));
}

void TraverseCast(Chao::CSD::Scene* scene, uint32_t castNodeIndex, Chao::CSD::CastNode* castNode, uint32_t castIndex, const std::string& parentPath)
{
    if (castIndex == ~0)
        return;

    TraverseCast(scene, castNodeIndex, castNode, castNode->pCastLinks[castIndex].SiblingCastIndex, parentPath);

    std::string path = parentPath;

    for (size_t i = 0; i < scene->CastCount; i++)
    {
        auto& index = scene->pCastIndices[i];
        if (index.CastNodeIndex == castNodeIndex && index.CastIndex == castIndex)
        {
            path += index.pCastName;
            break;
        }
    }

    EmplacePath(castNode->pCasts[castIndex].get(), path);

    if (castNode->RootCastIndex == castIndex)
        EmplacePath(castNode, path);

    path += "/";

    TraverseCast(scene, castNodeIndex, castNode, castNode->pCastLinks[castIndex].ChildCastIndex, path);

    // LOGFN_UTILITY("CSD hierarchy: {}", path);
}

void TraverseScene(Chao::CSD::Scene* scene, std::string path)
{
    EmplacePath(scene, path);
    path += "/";

    for (size_t i = 0; i < scene->CastNodeCount; i++)
    {
        auto& castNode = scene->pCastNodes[i];
        TraverseCast(scene, i, &castNode, castNode.RootCastIndex, path);
    }
}

void TraverseSceneNode(Chao::CSD::SceneNode* sceneNode, std::string path)
{
    EmplacePath(sceneNode, path);
    path += "/";

    for (size_t i = 0; i < sceneNode->SceneCount; i++)
    {
        auto& sceneIndex = sceneNode->pSceneIndices[i];
        TraverseScene(sceneNode->pScenes[sceneIndex.SceneIndex], path + sceneIndex.pSceneName.get());
    }

    for (size_t i = 0; i < sceneNode->SceneNodeCount; i++)
    {
        auto& sceneNodeIndex = sceneNode->pSceneNodeIndices[i];
        TraverseSceneNode(&sceneNode->pSceneNodes[sceneNodeIndex.SceneNodeIndex], path + sceneNodeIndex.pSceneNodeName.get());
    }
}

// Sonicteam::CsdResource::MakeResource
PPC_FUNC_IMPL(__imp__sub_82617570);
PPC_FUNC(sub_82617570)
{
    auto pName = reinterpret_cast<char*>(base + PPC_LOAD_U32(ctx.r4.u32 + 4));

    __imp__sub_82617570(ctx, base);

    if (!ctx.r3.u32)
        return;

    auto ppCsdObject = PPC_LOAD_U32(ctx.r3.u32);

    if (!ppCsdObject)
        return;

    auto pCsdObject = reinterpret_cast<Sonicteam::CsdObject*>(base + ppCsdObject);

    if (!pCsdObject || !pCsdObject->m_pCsdProject)
        return;

    LOGFN_UTILITY("CSD loaded: {} (0x{:08X})", pName, (uint64_t)pCsdObject->m_pCsdProject.get());

    static const char* s_languages[7] =
    {
        nullptr, // Maps to ELanguages enum.
        "_English",
        "_Japanese",
        "_German",
        "_French",
        "_Spanish",
        "_Italian"
    };

    auto pSuffix = s_languages[(int)Config::Language.Value];

    auto nameLen = strlen(pName);
    auto suffixLen = strlen(pSuffix);

    // Truncate language names to redirect CSD modifiers.
    if (suffixLen < nameLen)
    {
        if (strcmpIgnoreCase(pName + nameLen - suffixLen, pSuffix))
        {
            pName[nameLen - suffixLen] = '\0';

            LOGFN_UTILITY("CSD modifier(s) redirected: {}", pName);
        }
    }

    TraverseSceneNode(pCsdObject->m_pCsdProject->m_pResource->pRootNode, pName);
}

// Chao::CSD::CMemoryAlloc::Free
PPC_FUNC_IMPL(__imp__sub_82656650);
PPC_FUNC(sub_82656650)
{
    if (ctx.r4.u32 != NULL && PPC_LOAD_U32(ctx.r4.u32) == 0x4649584E && PPC_LOAD_U32(ctx.r4.u32 + 0x20) == 0x6E43504A) // NXIF, nCPJ
    {
        uint32_t fileSize = PPC_LOAD_U32(ctx.r4.u32 + 0x14);
    
        std::lock_guard lock(g_pathMutex);
        const uint8_t* key = base + ctx.r4.u32;
    
        auto lower = g_paths.lower_bound(key);
        auto upper = g_paths.lower_bound(key + fileSize);
    
        g_paths.erase(lower, upper);

        LOGFN_UTILITY("CSD freed: 0x{:08X}", (uint64_t)key);
    }

    __imp__sub_82656650(ctx, base);
}

// Chao::CSD::Scene::Render
PPC_FUNC_IMPL(__imp__sub_828C8F60);
PPC_FUNC(sub_828C8F60)
{
    auto pScene = (Chao::CSD::Scene*)(base + ctx.r3.u32);

    g_sceneModifier = FindCsdModifier(ctx.r3.u32);

    if (g_sceneModifier.has_value())
    {
        if ((g_sceneModifier->Flags & CSD_MODIFIER_ULTRAWIDE_ONLY) != 0 && g_aspectRatio <= WIDE_ASPECT_RATIO)
            g_sceneModifier->Flags &= (~g_sceneModifier->Flags) | CSD_MODIFIER_ULTRAWIDE_ONLY;

        if ((g_sceneModifier->Flags & CSD_SCENE_DISABLE_MOTION) != 0)
            pScene->FPS = 0;

        if (g_aspectRatio > WIDE_ASPECT_RATIO)
        {
            if ((g_sceneModifier->Flags & (CSD_OFFSET_SCALE_LEFT | CSD_OFFSET_SCALE_RIGHT | CSD_CORNER_EXTRACT)) != 0)
            {
                auto r3 = ctx.r3;
                auto r4 = ctx.r4;
                auto r5 = ctx.r5;
                auto r6 = ctx.r6;

                // Queue draw calls, but don't actually draw anything. We just want to extract the corner.
                g_cornerExtract = true;
                __imp__sub_828C8F60(ctx, base);
                g_cornerExtract = false;

#ifdef CORNER_DEBUG
                if (g_sceneModifier->CornerMax == FLT_MAX)
                {
                    fmt::print("Corners: ");
                    for (auto corner : g_corners)
                        fmt::print("{} ", corner);

                    fmt::println("");
                }
#endif

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
    g_castNodeModifier = FindCsdModifier(r10.u32 + r27.u32);
}

void RenderCsdCastMidAsmHook(PPCRegister& r4)
{
    g_castModifier = FindCsdModifier(r4.u32);
}

void Draw(PPCContext& ctx, uint8_t* base, PPCFunc* original, uint32_t stride)
{
    CsdModifier modifier{};

    auto vpWidth = float(Video::s_viewportWidth);
    auto vpHeight = float(Video::s_viewportHeight);

    if (g_castModifier.has_value())
    {
        modifier = g_castModifier.value();
    }
    else if (g_castNodeModifier.has_value())
    {
        modifier = g_castNodeModifier.value();
    }
    else if (g_sceneModifier.has_value())
    {
        modifier = g_sceneModifier.value();
    }

    // Remove all flags if the aspect ratio is above 16:9.
    if ((modifier.Flags & CSD_MODIFIER_ULTRAWIDE_ONLY) != 0 && g_aspectRatio <= WIDE_ASPECT_RATIO)
        modifier.Flags &= (~modifier.Flags) | CSD_MODIFIER_ULTRAWIDE_ONLY;

    // Remove all flags if the aspect ratio is below 16:9.
    if ((modifier.Flags & CSD_MODIFIER_NARROW_ONLY) != 0 && g_aspectRatio >= WIDE_ASPECT_RATIO)
        modifier.Flags &= (~modifier.Flags) | CSD_MODIFIER_NARROW_ONLY;
    
    if ((modifier.Flags & CSD_SKIP) != 0)
    {
        if ((modifier.Flags & CSD_CHEVRON) != 0)
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

    if (g_cornerExtract)
    {
        if ((modifier.Flags & (CSD_STORE_LEFT_CORNER | CSD_STORE_RIGHT_CORNER)) != 0)
        {
            uint32_t vertexIndex = ((modifier.Flags & CSD_STORE_LEFT_CORNER) != 0) ? 0 : 3;
            g_corners[modifier.CornerIndex] = *reinterpret_cast<be<float>*>(base + ctx.r4.u32 + vertexIndex * stride);
        }

        return;
    }

    // Draw black bars if this cast is drawn.
    if ((modifier.Flags & CSD_BLACK_BAR) != 0)
        BlackBar::g_isVisible = true;

    // Prohibit black bars from being drawn if this cast is drawn.
    if ((modifier.Flags & CSD_PROHIBIT_BLACK_BAR) != 0)
        BlackBar::g_isVisible = false;

    if (Config::UIAlignmentMode == EUIAlignmentMode::Centre || BlackBar::g_isVisible)
    {
        // Prevent chevron arrows from being unaligned by centred aspect ratio.
        if ((modifier.Flags & CSD_CHEVRON) == 0)
        {
            if (g_aspectRatio > WIDE_ASPECT_RATIO)
            {
                // Remove horizontal alignments at wide aspect ratios.
                modifier.Flags &= ~(CSD_ALIGN_LEFT | CSD_ALIGN_RIGHT);
            }
            else if (g_aspectRatio < WIDE_ASPECT_RATIO)
            {
                // Remove vertical alignments at narrow aspect ratios.
                modifier.Flags &= ~(CSD_ALIGN_TOP | CSD_ALIGN_BOTTOM);
            }
        }
    }

    // Reserve stack space for vertices.
    auto size = ctx.r5.u32 * stride;
    ctx.r1.u32 -= size;

    // Copy vertices to stack.
    auto stack = base + ctx.r1.u32;
    memcpy(stack, base + ctx.r4.u32, size);

    struct CSDVertex
    {
        be<float> X;
        be<float> Y;
        be<uint32_t> Colour;
        be<float> U;
        be<float> V;
    };

    auto getVertex = [&](size_t index)
    {
        return reinterpret_cast<CSDVertex*>(stack + (index * stride));
    };

    auto offsetX = 0.0f;
    auto offsetY = 0.0f;
    auto pivotX = 0.0f;
    auto pivotY = 0.0f;
    auto scaleX = 1.0f;
    auto scaleY = 1.0f;

    auto needsStretch = g_aspectRatio >= WIDE_ASPECT_RATIO;

    if (needsStretch && (modifier.Flags & CSD_STRETCH_HORIZONTAL) != 0)
    {
        scaleX = vpWidth / 1280.0f;
    }
    else
    {
        scaleX = g_aspectRatioScale;

        if (needsStretch && (modifier.Flags & CSD_UNSTRETCH_HORIZONTAL) != 0)
        {
            pivotX = getVertex(0)->X;
            offsetX = pivotX * vpWidth / 1280.0f;
        }
        else
        {
            if ((modifier.Flags & CSD_ALIGN_RIGHT) != 0)
            {
                offsetX = g_aspectRatioOffsetX * 2.0f;

                if ((modifier.Flags & CSD_MULTIPLAYER) != 0 && g_aspectRatio > WIDE_ASPECT_RATIO)
                    offsetX = g_aspectRatioMultiplayerOffsetX * 2.0f;
            }
            else if ((modifier.Flags & CSD_ALIGN_LEFT) == 0)
                offsetX = g_aspectRatioOffsetX;

            // Don't offset arrows at 16:9 or narrower.
            if ((modifier.Flags & CSD_CHEVRON) != 0 && g_aspectRatio <= WIDE_ASPECT_RATIO)
                offsetX = 0.0f;

            if ((modifier.Flags & CSD_SCALE) != 0)
            {
                scaleX *= g_aspectRatioGameplayScale;

                if ((modifier.Flags & CSD_ALIGN_RIGHT) != 0)
                    offsetX += 1280.0f * (1.0f - g_aspectRatioGameplayScale) * g_aspectRatioScale;
                else if ((modifier.Flags & CSD_ALIGN_LEFT) == 0)
                    offsetX += 640.0f * (1.0f - g_aspectRatioGameplayScale) * g_aspectRatioScale;

                offsetX += pivotX * g_aspectRatioScale;
            }
        }
    }

    if ((modifier.Flags & CSD_STRETCH_VERTICAL) != 0)
    {
        scaleY = vpHeight / 720.0f;
    }
    else
    {
        scaleY = g_aspectRatioScale;

        if ((modifier.Flags & CSD_ALIGN_BOTTOM) != 0)
            offsetY = g_aspectRatioOffsetY * 2.0f;
        else if ((modifier.Flags & CSD_ALIGN_TOP) == 0)
            offsetY = g_aspectRatioOffsetY;

        if ((modifier.Flags & CSD_SCALE) != 0)
        {
            scaleY *= g_aspectRatioGameplayScale;

            if ((modifier.Flags & CSD_ALIGN_BOTTOM) != 0)
                offsetY += 720.0f * (1.0f - g_aspectRatioGameplayScale) * g_aspectRatioScale;
            else if ((modifier.Flags & CSD_ALIGN_TOP) == 0)
                offsetY += 360.0f * (1.0f - g_aspectRatioGameplayScale) * g_aspectRatioScale;

            offsetY += pivotY * g_aspectRatioScale;
        }
    }

    // Offset cast to movie aspect ratio boundaries.
    if ((modifier.Flags & CSD_MOVIE) != 0)
    {
        if (g_aspectRatio > g_aspectRatioMovie)
        {
            offsetX -= (vpWidth - (vpHeight * g_aspectRatioMovie)) / 2.0f;
        }
        else
        {
            offsetY -= (vpHeight - (vpWidth / g_aspectRatioMovie)) / 2.0f;
        }
    }

    if (g_aspectRatio > WIDE_ASPECT_RATIO)
    {
        CsdModifier offsetScaleModifier{};

        auto corner = 0.0f;

        if (g_castModifier.has_value())
        {
            offsetScaleModifier = g_castModifier.value();
            corner = getVertex(((offsetScaleModifier.Flags & CSD_STORE_LEFT_CORNER) != 0) ? 0 : 3)->X;
        }

        if (offsetScaleModifier.CornerMax == 0.0f && g_castNodeModifier.has_value())
        {
            offsetScaleModifier = g_castNodeModifier.value();
            corner = g_corners[offsetScaleModifier.CornerIndex];
        }

        if (offsetScaleModifier.CornerMax == 0.0f && g_sceneModifier.has_value())
        {
            offsetScaleModifier = g_sceneModifier.value();
            corner = g_corners[offsetScaleModifier.CornerIndex];
        }

#ifdef CORNER_DEBUG
        if ((offsetScaleModifier.Flags & (CSD_OFFSET_SCALE_LEFT | CSD_OFFSET_SCALE_RIGHT)) != 0 && offsetScaleModifier.CornerMax == FLT_MAX)
            fmt::println("Corner: {}", corner);
#endif

        if ((offsetScaleModifier.Flags & CSD_OFFSET_SCALE_LEFT) != 0)
            offsetX *= corner / offsetScaleModifier.CornerMax;
        else if ((offsetScaleModifier.Flags & CSD_OFFSET_SCALE_RIGHT) != 0)
            offsetX = vpWidth - (vpWidth - offsetX) * (1280.0f - corner) / (1280.0f - offsetScaleModifier.CornerMax);
    }

    auto firstX = 0.0f;
    auto firstY = 0.0f;
    auto lastX = 0.0f;
    auto lastY = 0.0f;
    auto width = 0.0f;
    auto height = 0.0f;

    for (size_t i = 0; i < ctx.r5.u32; i++)
    {
        auto vertex = getVertex(i);

        auto x = offsetX + (vertex->X - pivotX) * scaleX;
        auto y = offsetY + (vertex->Y - pivotY) * scaleY;

        if ((modifier.Flags & CSD_EXTEND_LEFT) != 0 && (i == 0 || i == 1))
        {
            x = std::min(x, 0.0f);
        }
        else if ((modifier.Flags & CSD_EXTEND_RIGHT) != 0 && (i == 2 || i == 3))
        {
            // Preserve Audio Room "pod" base right edge.
            if ((modifier.Flags & CSD_POD_BASE) != 0)
                g_podBaseRightX = x;

            x = std::max(x, vpWidth);
        }

        switch (i)
        {
            case 0: firstX = x; break;
            case 1: lastY  = y; break;
            case 2: firstY = y; break;
            case 3: lastX  = x; break;
        }

        if ((modifier.Flags & CSD_RADARMAP) != 0)
        {
            g_radarMapX = x;
            g_radarMapY = y;
        }

        vertex->X = round(x);
        vertex->Y = round(y);
    }

    width = lastX - firstX;
    height = lastY - firstY;

    if ((modifier.Flags & CSD_RADARMAP) != 0)
    {
        g_radarMapCoverWidth = width;
        g_radarMapCoverHeight = height;
    }

    auto flipHorizontally = [&]()
    {
        getVertex(0)->X = getVertex(0)->X + width;
        getVertex(1)->X = getVertex(1)->X + width;
        getVertex(2)->X = getVertex(2)->X - width;
        getVertex(3)->X = getVertex(3)->X - width;
    };

    auto flipVertically = [&]()
    {
        getVertex(0)->Y = getVertex(0)->Y + height;
        getVertex(1)->Y = getVertex(1)->Y - height;
        getVertex(2)->Y = getVertex(2)->Y + height;
        getVertex(3)->Y = getVertex(3)->Y - height;
    };

    auto applyUVModifier = [&](CsdUVs uvModifier)
    {
        getVertex(0)->U = getVertex(0)->U + uvModifier.U0;
        getVertex(0)->V = getVertex(0)->V + uvModifier.V0;
        getVertex(1)->U = getVertex(1)->U + uvModifier.U1;
        getVertex(1)->V = getVertex(1)->V + uvModifier.V1;
        getVertex(2)->U = getVertex(2)->U + uvModifier.U2;
        getVertex(2)->V = getVertex(2)->V + uvModifier.V2;
        getVertex(3)->U = getVertex(3)->U + uvModifier.U3;
        getVertex(3)->V = getVertex(3)->V + uvModifier.V3;
    };

    auto applyColourModifier = [&](CsdColours colourModifier)
    {
        getVertex(0)->Colour = colourModifier.C0;
        getVertex(1)->Colour = colourModifier.C1;
        getVertex(2)->Colour = colourModifier.C2;
        getVertex(3)->Colour = colourModifier.C3;
    };

    if ((modifier.Flags & CSD_UV_MODIFIER) != 0)
        applyUVModifier(modifier.UVs);

    if ((modifier.Flags & CSD_COLOUR_MODIFIER) != 0)
        applyColourModifier(modifier.Colours);

    // Don't repeat arrows at 16:9 or narrower.
    if ((modifier.Flags & CSD_CHEVRON) != 0 && g_aspectRatio <= WIDE_ASPECT_RATIO)
        modifier.Flags &= ~(CSD_REPEAT_LEFT | CSD_REPEAT_RIGHT);

    auto isRepeatLeft = (modifier.Flags & CSD_REPEAT_LEFT) != 0;
    auto isRepeatRight = (modifier.Flags & CSD_REPEAT_RIGHT) != 0;

    auto r3 = ctx.r3;
    auto r5 = ctx.r5;

    if (isRepeatLeft || isRepeatRight)
    {
        auto isFlipHorz = (modifier.Flags & CSD_REPEAT_FLIP_HORIZONTAL) != 0;
        auto isFlipVert = (modifier.Flags & CSD_REPEAT_FLIP_VERTICAL) != 0;

        auto applyRepeatModifiers = [&]()
        {
            if (isFlipHorz)
                flipHorizontally();

            if (isFlipVert)
                flipVertically();

            if ((modifier.Flags & CSD_REPEAT_UV_MODIFIER) != 0)
                applyUVModifier(modifier.RepeatUVs);

            if ((modifier.Flags & CSD_REPEAT_COLOUR_MODIFIER) != 0)
                applyColourModifier(modifier.RepeatColours);
        };

        if (isRepeatLeft)
        {
            auto x = getVertex(2)->X;
            auto arrowIndex = 0;
            auto arrowCount = 0;
            auto arrowWidth = (width - (width / 2.5f));

            if ((modifier.Flags & CSD_CHEVRON) != 0)
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

            while (x > 0.0f)
            {
                ctx.r3 = r3;
                ctx.r4 = ctx.r1;
                ctx.r5 = r5;
                original(ctx, base);

                if ((modifier.Flags & CSD_CHEVRON) != 0)
                {
                    ///////////////////////////////////////////////////////////
                    // Clone foreground arrows to recreate chevron animation //
                    ///////////////////////////////////////////////////////////

                    // These values are based off the animation in background.xncp.
                    auto endAlpha = (uint8_t)std::lerp(30, 10, x / vpWidth);
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
                        getVertex(i)->Colour = 0xFFFFFF00 | g_fgArrows[arrowIndex].CurrentAlpha;
                    }

                    arrowIndex++;
                }
                else
                {
                    // Move cloned cast to left edge of existing cast.
                    for (size_t i = 0; i < r5.u32; i++)
                        getVertex(i)->X = getVertex(i)->X - width;
                }

                // Update loop condition.
                x = getVertex(2)->X;

                applyRepeatModifiers();

                if ((modifier.Flags & CSD_REPEAT_EXTEND) != 0)
                {
                    for (size_t i = 0; i < r5.u32; i++)
                    {
                        if (i == (isFlipHorz ? 2 : 0) || i == (isFlipHorz ? 3 : 1))
                            getVertex(i)->X = std::min(getVertex(i)->X.get(), 0.0f);
                    }
                }
            }
        }

        if (isRepeatRight)
        {
            auto x = getVertex(0)->X;
            auto arrowIndex = 0;
            auto arrowCount = 0;
            auto arrowWidth = (width - (width / 4.0f));

            if ((modifier.Flags & CSD_CHEVRON) != 0)
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

            while (x < vpWidth)
            {
                ctx.r3 = r3;
                ctx.r4 = ctx.r1;
                ctx.r5 = r5;
                original(ctx, base);

                if ((modifier.Flags & CSD_CHEVRON) != 0)
                {
                    ///////////////////////////////////////////////////////////
                    // Clone background arrows to recreate chevron animation //
                    ///////////////////////////////////////////////////////////

                    // These values are based off the animation in background.xncp.
                    auto endAlpha = (uint8_t)std::lerp(10, 35, x / vpWidth);
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
                        getVertex(i)->Colour = 0xFFFFFF00 | g_bgArrows[arrowIndex].CurrentAlpha;
                    }

                    arrowIndex++;
                }
                else
                {
                    // Move cloned cast to right edge of existing cast.
                    for (size_t i = 0; i < r5.u32; i++)
                        getVertex(i)->X = getVertex(i)->X + width;
                }

                // Update loop condition.
                x = getVertex(0)->X;

                applyRepeatModifiers();

                // Extend to the right of the screen.
                if ((modifier.Flags & CSD_REPEAT_EXTEND) != 0)
                {
                    for (size_t i = 0; i < r5.u32; i++)
                    {
                        if (i == (isFlipHorz ? 0 : 2) || i == (isFlipHorz ? 1 : 3))
                            getVertex(i)->X = std::max(getVertex(i)->X.get(), vpWidth);
                    }
                }
            }
        }
    }
    else
    {
        ctx.r3 = r3;
        ctx.r4 = ctx.r1;
        ctx.r5 = r5;
        original(ctx, base);

        // Clone Audio Room "pod" left edge to fill
        // in the rest of the box at ultrawide.
        if ((modifier.Flags & CSD_POD_CLONE) != 0 && g_aspectRatio > WIDE_ASPECT_RATIO)
        {
            auto v0 = getVertex(0);
            auto v1 = getVertex(1);
            auto v2 = getVertex(2);
            auto v3 = getVertex(3);

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

            ctx.r3 = r3;
            ctx.r4 = ctx.r1;
            ctx.r5 = r5;
            original(ctx, base);
        }

        // Don't adjust CRI logo at 16:9 or if the alignment mode is centre at ultrawide.
        if (g_aspectRatio >= WIDE_ASPECT_RATIO && Config::UIAlignmentMode == EUIAlignmentMode::Centre)
            modifier.Flags &= ~CSD_CRI_LOGO;

        // Clone CRI logo to move the technology
        // logos to the true bottom right corner.
        if ((modifier.Flags & CSD_CRI_LOGO) != 0)
        {
            auto v0 = getVertex(0);
            auto v1 = getVertex(1);
            auto v2 = getVertex(2);
            auto v3 = getVertex(3);

            auto originalTop = v0->Y;
            auto originalHeight = v1->Y - v0->Y;

            auto width = 422.0f;
            auto height = 132.0f;
            auto widthScaled = width * g_aspectRatioScale;
            auto heightScaled = height * g_aspectRatioScale;

            /////////////////////////////////////////////////////////////////////////
            // Clone and scale white square to block the original technology logos //
            /////////////////////////////////////////////////////////////////////////

            auto top = originalTop + (originalHeight - heightScaled);
            auto left = g_aspectRatio >= WIDE_ASPECT_RATIO ? 0.0f : vpWidth - widthScaled;

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

            ctx.r3 = r3;
            ctx.r4 = ctx.r1;
            ctx.r5 = r5;
            original(ctx, base);

            /////////////////////////////////////////////
            // Clone and scale to fit technology logos //
            /////////////////////////////////////////////

            v0 = getVertex(0);
            v1 = getVertex(1);
            v2 = getVertex(2);
            v3 = getVertex(3);

            auto uv = PIXELS_TO_UV_COORDS(2048, 512, 858, 380, width, height);
            auto& min = std::get<0>(uv);
            auto& max = std::get<1>(uv);

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

            ctx.r3 = r3;
            ctx.r4 = ctx.r1;
            ctx.r5 = r5;
            original(ctx, base);
        }
    }

    if (g_aspectRatio > WIDE_ASPECT_RATIO)
    {
        /////////////////////////////////////////////////
        //  Scale metal borders to wide aspect ratios  //
        /////////////////////////////////////////////////

        if ((modifier.Flags & CSD_MAIN_MENU_PARTS_CAST_0222) != 0)
        {
            // Move cloned cast to left edge of existing cast.
            for (size_t i = 0; i < r5.u32; i++)
                getVertex(i)->X = getVertex(i)->X - width;

            flipHorizontally();

            // Extend to the left of the screen.
            getVertex(2)->X = 0.0f;
            getVertex(3)->X = 0.0f;

            // Crop UVs to remove metal notch.
            getVertex(0)->U = getVertex(0)->U + 0.0015f;
            getVertex(1)->U = getVertex(1)->U + 0.0015f;
            getVertex(2)->U = getVertex(2)->U + -0.1f;
            getVertex(3)->U = getVertex(3)->U + -0.1f;

            ctx.r3 = r3;
            ctx.r4 = ctx.r1;
            ctx.r5 = r5;
            original(ctx, base);
        }

        if ((modifier.Flags & CSD_MAIN_MENU_PARTS_CAST_0226) != 0)
        {
            // Move cloned cast to left edge of existing cast.
            for (size_t i = 0; i < r5.u32; i++)
                getVertex(i)->X = getVertex(i)->X - width;

            flipHorizontally();

            // Extend to the left of the screen.
            getVertex(2)->X = 0.0f;
            getVertex(3)->X = 0.0f;

            // Crop UVs to remove metal dip.
            getVertex(2)->U = getVertex(2)->U + -0.8f;
            getVertex(3)->U = getVertex(3)->U + -0.8f;

            ctx.r3 = r3;
            ctx.r4 = ctx.r1;
            ctx.r5 = r5;
            original(ctx, base);
        }

        if ((modifier.Flags & CSD_MAIN_MENU_PARTS_CAST_0227) != 0)
        {
            // Move cloned cast to right edge of existing cast.
            for (size_t i = 0; i < r5.u32; i++)
                getVertex(i)->X = getVertex(i)->X + width;

            flipHorizontally();

            // Extend to the right of the screen.
            getVertex(0)->X = vpWidth;
            getVertex(1)->X = vpWidth;

            // Crop UVs to remove metal dip.
            getVertex(0)->U = getVertex(0)->U + -0.8f;
            getVertex(1)->U = getVertex(1)->U + -0.8f;

            ctx.r3 = r3;
            ctx.r4 = ctx.r1;
            ctx.r5 = r5;
            original(ctx, base);
        }
    }
    else
    {
        /////////////////////////////////////////////////
        // Scale metal borders to narrow aspect ratios //
        /////////////////////////////////////////////////

        if ((modifier.Flags & (CSD_MAIN_MENU_PARTS_CAST_0221 | CSD_MAIN_MENU_PARTS_CAST_0222)) != 0)
        {
            // Move cloned cast above existing cast.
            for (size_t i = 0; i < r5.u32; i++)
                getVertex(i)->Y = getVertex(i)->Y - height + 6.0f;

            flipVertically();

            // Extend to the top of the screen
            // if the first clone no longer fits.
            if (getVertex(1)->Y - height > 0.0f)
            {
                getVertex(1)->Y = 0.0f;
                getVertex(3)->Y = 0.0f;
            }

            // Crop UVs to remove metal notch.
            getVertex(0)->V = getVertex(0)->V + 0.005f;
            getVertex(1)->V = getVertex(1)->V + -0.075f;
            getVertex(2)->V = getVertex(2)->V + 0.005f;
            getVertex(3)->V = getVertex(3)->V + -0.075f;

            ctx.r3 = r3;
            ctx.r4 = ctx.r1;
            ctx.r5 = r5;
            original(ctx, base);
        }

        if ((modifier.Flags & (CSD_MAIN_MENU_PARTS_CAST_0226 | CSD_MAIN_MENU_PARTS_CAST_0227)) != 0)
        {
            // Move cloned cast below existing cast.
            for (size_t i = 0; i < r5.u32; i++)
                getVertex(i)->Y = getVertex(i)->Y + height - 18.0f;

            flipVertically();

            // Extend to the bottom of the screen
            // if the first clone no longer fits.
            if (getVertex(0)->Y - height < vpHeight)
            {
                getVertex(0)->Y = vpHeight;
                getVertex(2)->Y = vpHeight;
            }

            // Crop UVs to remove metal dip.
            getVertex(0)->V = getVertex(0)->V + 0.065f;
            getVertex(1)->V = getVertex(1)->V + -0.0175f;
            getVertex(2)->V = getVertex(2)->V + 0.065f;
            getVertex(3)->V = getVertex(3)->V + -0.0175f;

            ctx.r3 = r3;
            ctx.r4 = ctx.r1;
            ctx.r5 = r5;
            original(ctx, base);
        }
    }

    ctx.r1.u32 += size;
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
        BlackBar::g_isVisible = true;

    __imp__sub_8264AC48(ctx, base);
}

// Sonicteam::HUDResult::Update
PPC_FUNC_IMPL(__imp__sub_824F4D80);
PPC_FUNC(sub_824F4D80)
{
    BlackBar::g_isVisible = true;

    __imp__sub_824F4D80(ctx, base);
}

// Sonicteam::HUDBattleResult::Update
PPC_FUNC_IMPL(__imp__sub_824D32C8);
PPC_FUNC(sub_824D32C8)
{
    BlackBar::g_isVisible = true;

    __imp__sub_824D32C8(ctx, base);
}

// Sonicteam::EndingMode::Update
PPC_FUNC_IMPL(__imp__sub_824A4A40);
PPC_FUNC(sub_824A4A40)
{
    BlackBar::g_isVisible = true;

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
    auto pHUDRaderMap = (Sonicteam::HUDRaderMap*)(base + ctx.r3.u32);

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
    auto pCObjBalloonIconDrawable = (Sonicteam::CObjBalloonIconDrawable*)(base + ctx.r3.u32);
    auto scale = g_aspectRatioScale;

    if (g_aspectRatio > WIDE_ASPECT_RATIO)
        scale = g_aspectRatio / WIDE_ASPECT_RATIO;

    pCObjBalloonIconDrawable->m_Vertices[0].X = -1.0f / scale;
    pCObjBalloonIconDrawable->m_Vertices[0].Y = 0.0f;
    pCObjBalloonIconDrawable->m_Vertices[1].X = -1.0f / scale;
    pCObjBalloonIconDrawable->m_Vertices[1].Y = g_aspectRatio / scale;
    pCObjBalloonIconDrawable->m_Vertices[2].X = 1.0f / scale;
    pCObjBalloonIconDrawable->m_Vertices[2].Y = 0.0f;
    pCObjBalloonIconDrawable->m_Vertices[3].X = 1.0f / scale;
    pCObjBalloonIconDrawable->m_Vertices[3].Y = g_aspectRatio / scale;

    __imp__sub_82352220(ctx, base);
}

// Sonicteam::MovieObjectWmv::Draw
PPC_FUNC_IMPL(__imp__sub_8264CC90);
PPC_FUNC(sub_8264CC90)
{
    auto pMovieObjectWmv = (Sonicteam::MovieObjectWmv*)(base + ctx.r3.u32);

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

    auto movieModifier = FindMovieModifier(movieNameHash);

    g_aspectRatioMovie = (float)pMovieObjectWmv->m_Width / (float)pMovieObjectWmv->m_Height;

    float width, height, left, right, top, bottom;

    if (s_movieUsesCustomDimensions)
    {
        auto movieRectLeft = s_movieLeft;
        auto movieRectRight = s_movieRight;
        auto movieRectTop = s_movieTop;
        auto movieRectBottom = s_movieBottom;

        if (g_aspectRatio > g_aspectRatioMovie)
        {
            // Scale width at wide aspect ratios.
            movieRectLeft = s_movieLeft / (g_aspectRatio / g_aspectRatioMovie);
            movieRectRight = s_movieRight / (g_aspectRatio / g_aspectRatioMovie);
        }
        else
        {
            // Scale height at narrow aspect ratios.
            movieRectTop = s_movieTop / (g_aspectRatioMovie / g_aspectRatio);
            movieRectBottom = s_movieBottom / (g_aspectRatioMovie / g_aspectRatio);
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

        if (g_aspectRatio > g_aspectRatioMovie)
        {
            if ((movieModifier.Flags & MOVIE_CROP_WIDE) != 0)
            {
                // Crop vertically at wide aspect ratios.
                height = 2.0f * (g_aspectRatio / g_aspectRatioMovie);
            }
            else
            {
                // Pillarbox at wide aspect ratios.
                width = 2.0f * (g_aspectRatioMovie / g_aspectRatio);
            }
        }
        else
        {
            if ((movieModifier.Flags & MOVIE_CROP_NARROW) != 0)
            {
                // Crop horizontally at narrow aspect ratios.
                width = 2.0f * (g_aspectRatioMovie / g_aspectRatio);
            }
            else
            {
                // Letterbox at narrow aspect ratios.
                height = 2.0f * (g_aspectRatio / g_aspectRatioMovie);
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
    if (!pTextEntity || !pTextEntity->m_ImageVertexCount)
        return;

    auto variables = MapTextVariables(pTextEntity->m_pVariables);
    auto variablesIndex = 0;
    auto pictureIndex = 0;

    for (int i = 0; i < pTextEntity->m_Text.size(); i++)
    {
        auto str = pTextEntity->m_Text.c_str();
        auto c = ByteSwap(str[i]);

        if (c != L'$')
            continue;

        // Some strings may have more placeholder
        // characters than variables in the map.
        if (variablesIndex >= variables.size())
            break;

        auto& variable = variables[variablesIndex];

        if (variable.first == "picture")
        {
            auto w = g_textFontPicture.m_TextureWidth;
            auto h = g_textFontPicture.m_TextureHeight;

            auto isPlayStation = Config::ControllerIcons == EControllerIcons::PlayStation;

            if (Config::ControllerIcons == EControllerIcons::Auto)
                isPlayStation = hid::g_inputDeviceController == hid::EInputDevice::PlayStation;

            auto& pftModifier = isPlayStation
                ? g_pftModifierPS3
                : g_pftModifierXenon;

            auto baseParams = FindFontPictureModifier(g_pftModifierXenon, variable.second);
            auto newParams = FindFontPictureModifier(pftModifier, variable.second);

            auto  uv = PIXELS_TO_UV_COORDS(w, h, newParams.X, newParams.Y, newParams.Width, newParams.Height);
            auto& min = std::get<0>(uv);
            auto& max = std::get<1>(uv);

            auto  vi = pictureIndex * 6;
            auto& v0 = pTextEntity->m_pImageVertices[vi + 0];
            auto& v1 = pTextEntity->m_pImageVertices[vi + 1];
            auto& v2 = pTextEntity->m_pImageVertices[vi + 2];
            auto& v3 = pTextEntity->m_pImageVertices[vi + 3];
            auto& v4 = pTextEntity->m_pImageVertices[vi + 4];
            auto& v5 = pTextEntity->m_pImageVertices[vi + 5];

            auto centreX = (v0.X + v2.X) / 2.0f;
            auto widthAdjust = (float)newParams.Width / (float)baseParams.Width;
            auto adjust = [&](auto& vertex)
            {
                vertex.X = centreX + (vertex.X - centreX) * widthAdjust;
            };

            // Bottom Left (Triangle 1)
            adjust(v0);
            v0.U = min.x;
            v0.V = min.y;

            // Top Left (Triangle 1)
            adjust(v1);
            v1.U = min.x;
            v1.V = max.y;

            // Top Right (Triangle 1)
            adjust(v2);
            v2.U = max.x;
            v2.V = max.y;

            // Bottom Left (Triangle 2)
            adjust(v3);
            v3.U = min.x;
            v3.V = min.y;

            // Top Right (Triangle 2)
            adjust(v4);
            v4.U = max.x;
            v4.V = max.y;

            // Bottom Right (Triangle 2)
            adjust(v5);
            v5.U = max.x;
            v5.V = min.y;

            pictureIndex++;
        }

        variablesIndex++;
    }
}

void TextEntityAlloc(PPCRegister& r3)
{
    r3.u32 += sizeof(uint64_t);
}

// Sonicteam::TextEntity::TextEntity
PPC_FUNC_IMPL(__imp__sub_8262DC60);
PPC_FUNC(sub_8262DC60)
{
    auto pTextModifier = (uint64_t*)(base + ctx.r3.u32 + sizeof(Sonicteam::TextEntity));

    // Default text modifier to scale.
    *pTextModifier = CSD_SCALE;

    __imp__sub_8262DC60(ctx, base);
}

// Sonicteam::TextEntity::Draw
PPC_FUNC_IMPL(__imp__sub_8262D868);
PPC_FUNC(sub_8262D868)
{
    auto pTextEntity = (Sonicteam::TextEntity*)(base + ctx.r3.u32);
    auto textModifier = *(uint64_t*)(base + ctx.r3.u32 + sizeof(Sonicteam::TextEntity));

    if ((textModifier & CSD_SKIP) != 0)
        return;

    auto x = pTextEntity->m_X;
    auto y = pTextEntity->m_Y;
    auto scaleX = pTextEntity->m_ScaleX;
    auto scaleY = pTextEntity->m_ScaleY;

    auto offsetX = 0.0f;
    auto offsetY = 0.0f;
    auto scale = g_aspectRatioScale;

    if (Config::UIAlignmentMode == EUIAlignmentMode::Centre || BlackBar::g_isVisible)
    {
        if (g_aspectRatio > WIDE_ASPECT_RATIO)
        {
            textModifier &= ~(CSD_ALIGN_LEFT | CSD_ALIGN_RIGHT);
        }
        else if (g_aspectRatio < WIDE_ASPECT_RATIO)
        {
            textModifier &= ~(CSD_ALIGN_TOP | CSD_ALIGN_BOTTOM);
        }
    }

    if ((textModifier & CSD_ALIGN_RIGHT) != 0)
        offsetX = g_aspectRatioOffsetX * 2.0f;
    else if ((textModifier & CSD_ALIGN_LEFT) == 0)
        offsetX = g_aspectRatioOffsetX;

    if ((textModifier & CSD_ALIGN_BOTTOM) != 0)
        offsetY = g_aspectRatioOffsetY * 2.0f;
    else if ((textModifier & CSD_ALIGN_TOP) == 0)
        offsetY = g_aspectRatioOffsetY;

    if ((textModifier & CSD_SCALE) != 0)
    {
        scale *= g_aspectRatioGameplayScale;

        if ((textModifier & CSD_ALIGN_RIGHT) != 0)
            offsetX += 1280.0f * (1.0f - g_aspectRatioGameplayScale) * g_aspectRatioScale;
        else if ((textModifier & CSD_ALIGN_LEFT) == 0)
            offsetX += 640.0f * (1.0f - g_aspectRatioGameplayScale) * g_aspectRatioScale;

        if ((textModifier & CSD_ALIGN_BOTTOM) != 0)
            offsetY += 720.0f * (1.0f - g_aspectRatioGameplayScale) * g_aspectRatioScale;
        else if ((textModifier & CSD_ALIGN_TOP) == 0)
            offsetY += 360.0f * (1.0f - g_aspectRatioGameplayScale) * g_aspectRatioScale;
    }

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

void SetTextEntityModifier(Sonicteam::TextEntity* pTextEntity, uint64_t flags)
{
    if (!pTextEntity)
        return;

    auto pTextModifier = (uint64_t*)(reinterpret_cast<uint8_t*>(pTextEntity) + sizeof(Sonicteam::TextEntity));

    *pTextModifier = flags;

    pTextEntity->m_FieldDD = true;
    pTextEntity->Update();
}

// Sonicteam::HUDMainMenu::Update
PPC_FUNC_IMPL(__imp__sub_824E11D0);
PPC_FUNC(sub_824E11D0)
{
    auto pHUDMainMenu = (Sonicteam::HUDMainMenu*)(base + ctx.r3.u32);
    auto pHudTextRoot = pHUDMainMenu->m_pHudTextRoot->m_pNext;

    static bool s_preservedTextPositions{};
    static float s_multiplayerTextOffsetX{};
    static float s_tagTextOffsetX{};
    static float s_battleTextOffsetX{};

    auto isTrialSelect = MainMenuTaskPatches::State >= 12 && MainMenuTaskPatches::State <= 15;
    auto isTag = MainMenuTaskPatches::State == Sonicteam::MainMenuTask::MainMenuState_Tag;

    while (pHudTextRoot)
    {
        if (pHudTextRoot->m_SceneName == "main_menu")
        {
            if (pHudTextRoot->m_CastName == "multi_player")
            {
                if (!s_preservedTextPositions)
                    s_multiplayerTextOffsetX = pHudTextRoot->m_OffsetX;

                pHudTextRoot->m_OffsetX = isTrialSelect ? -10000.0f : s_multiplayerTextOffsetX;
            }
            else if (pHudTextRoot->m_CastName == "tag")
            {
                if (!s_preservedTextPositions)
                    s_tagTextOffsetX = pHudTextRoot->m_OffsetX;

                pHudTextRoot->m_OffsetX = (isTrialSelect || isTag) ? -10000.0f : s_tagTextOffsetX;
            }
            else if (pHudTextRoot->m_CastName == "battle")
            {
                if (!s_preservedTextPositions)
                    s_battleTextOffsetX = pHudTextRoot->m_OffsetX;

                pHudTextRoot->m_OffsetX = isTrialSelect ? -10000.0f : s_battleTextOffsetX;
            }
        }

        pHudTextRoot = pHudTextRoot->m_pNext;
    }

    s_preservedTextPositions = true;

    __imp__sub_824E11D0(ctx, base);
}

// Sonicteam::HUDMainDisplay::Update
PPC_FUNC_IMPL(__imp__sub_824DCF40);
PPC_FUNC(sub_824DCF40)
{
    auto pHUDMainDisplay = (Sonicteam::HUDMainDisplay*)(base + ctx.r3.u32);

    SetTextEntityModifier(pHUDMainDisplay->m_Field184.get(), CSD_ALIGN_RIGHT | CSD_SCALE);
    SetTextEntityModifier(pHUDMainDisplay->m_spTrickPointText.get(), CSD_ALIGN_RIGHT | CSD_SCALE);
    SetTextEntityModifier(pHUDMainDisplay->m_Field194.get(), CSD_ALIGN_RIGHT | CSD_SCALE);
    SetTextEntityModifier(pHUDMainDisplay->m_spSavePointTimeText.get(), CSD_ALIGN_BOTTOM | CSD_SCALE);

    __imp__sub_824DCF40(ctx, base);
}

// Sonicteam::HintWindowTask::Update
PPC_FUNC_IMPL(__imp__sub_824D12F0);
PPC_FUNC(sub_824D12F0)
{
    auto pHintWindowTask = (Sonicteam::HintWindowTask*)(base + ctx.r3.u32);

    SetTextEntityModifier(pHintWindowTask->m_Field8C.get(), CSD_ALIGN_BOTTOM | CSD_SCALE);
    SetTextEntityModifier(pHintWindowTask->m_Field94.get(), CSD_ALIGN_BOTTOM | CSD_SCALE);

    __imp__sub_824D12F0(ctx, base);
}

// Sonicteam::MessageWindowTask::Update
PPC_FUNC_IMPL(__imp__sub_82507E68);
PPC_FUNC(sub_82507E68)
{
    auto pMessageWindowTask = (Sonicteam::MessageWindowTask*)(base + ctx.r3.u32);

    SetTextEntityModifier(pMessageWindowTask->m_Field90.get(), CSD_ALIGN_BOTTOM | CSD_SCALE);
    SetTextEntityModifier(pMessageWindowTask->m_Field98.get(), CSD_ALIGN_BOTTOM | CSD_SCALE);

    __imp__sub_82507E68(ctx, base);
}

// Sonicteam::TextFontPicture::LoadResource
PPC_FUNC_IMPL(__imp__sub_8263CC40);
PPC_FUNC(sub_8263CC40)
{
    auto pTextFontPicture = (Sonicteam::TextFontPicture*)(base + ctx.r3.u32);

    __imp__sub_8263CC40(ctx, base);

    g_textFontPicture = *pTextFontPicture;
}

// -------------- CSD MODIFIERS --------------- //

const xxHashMap<CsdModifier> g_csdModifiers =
{
    // audio
    { HashStr("sprite/audio/audio/pod/pod"), { CSD_SCALE } },
    { HashStr("sprite/audio/audio/pod/pod/Cast_1084"), { CSD_POD_BASE | CSD_SCALE | CSD_EXTEND_RIGHT } },
    { HashStr("sprite/audio/audio/pod/pod/Cast_1085"), { CSD_SCALE } },
    { HashStr("sprite/audio/audio/pod/pod/Cast_1086"), { CSD_POD_CLONE | CSD_SCALE } },
    { HashStr("sprite/audio/audio/pod/pod/Cast_1087"), { CSD_SCALE } },
    { HashStr("sprite/audio/audio/pod/pod/Cast_1088"), { CSD_SCALE | CSD_EXTEND_RIGHT } },
    { HashStr("sprite/audio/audio/genre"), { CSD_SCALE } },
    { HashStr("sprite/audio/audio/genre_cursor"), { CSD_SCALE } },
    { HashStr("sprite/audio/audio/audio_cursor"), { CSD_SCALE } },
    { HashStr("sprite/audio/audio/audio_cursor2"), { CSD_SCALE } },

    // background
    { HashStr("sprite/background/background/mainmenu_back"), { CSD_STRETCH_HORIZONTAL | CSD_PROHIBIT_BLACK_BAR } },
    { HashStr("sprite/background/background/tag"), { CSD_SCALE } },
    { HashStr("sprite/background/background/movie"), { CSD_SCALE } },
    { HashStr("sprite/background/background/main_menu"), { CSD_SCENE_DISABLE_MOTION | CSD_MODIFIER_ULTRAWIDE_ONLY } },
    { HashStr("sprite/background/background/main_menu/main_menu1/yaji1"), { CSD_CHEVRON | CSD_SCALE | CSD_SKIP } },
    { HashStr("sprite/background/background/main_menu/main_menu1/yaji2"), { CSD_CHEVRON | CSD_SCALE | CSD_SKIP } },
    { HashStr("sprite/background/background/main_menu/main_menu1/yaji3"), { CSD_CHEVRON | CSD_SCALE | CSD_SKIP } },
    { HashStr("sprite/background/background/main_menu/main_menu1/yaji4"), { CSD_CHEVRON | CSD_SCALE | CSD_SKIP } },
    { HashStr("sprite/background/background/main_menu/main_menu1/yaji5"), { CSD_CHEVRON | CSD_SCALE | CSD_SKIP } },
    { HashStr("sprite/background/background/main_menu/main_menu1/yaji6"), { CSD_CHEVRON | CSD_SCALE | CSD_ALIGN_RIGHT | CSD_REPEAT_LEFT } },
    { HashStr("sprite/background/background/main_menu/main_menu2/yaji7"), { CSD_CHEVRON | CSD_SCALE | CSD_SKIP } },
    { HashStr("sprite/background/background/main_menu/main_menu2/yaji8"), { CSD_CHEVRON | CSD_SCALE | CSD_SKIP } },
    { HashStr("sprite/background/background/main_menu/main_menu2/yaji9"), { CSD_CHEVRON | CSD_SCALE | CSD_SKIP } },
    { HashStr("sprite/background/background/main_menu/main_menu2/yaji10"), { CSD_CHEVRON | CSD_SCALE | CSD_SKIP } },
    { HashStr("sprite/background/background/main_menu/main_menu2/yaji11"), { CSD_CHEVRON | CSD_SCALE | CSD_SKIP } },
    { HashStr("sprite/background/background/main_menu/main_menu2/yaji12"), { CSD_CHEVRON | CSD_SCALE | CSD_SKIP } },
    { HashStr("sprite/background/background/main_menu/main_menu2/yaji13"), { CSD_CHEVRON | CSD_SCALE | CSD_SKIP } },
    { HashStr("sprite/background/background/main_menu/main_menu2/yaji14"), { CSD_CHEVRON | CSD_SCALE | CSD_SKIP } },
    { HashStr("sprite/background/background/main_menu/main_menu2/yaji15"), { CSD_CHEVRON | CSD_SCALE | CSD_ALIGN_LEFT | CSD_REPEAT_RIGHT } },
    { HashStr("sprite/background/background/cd"), { CSD_SCALE } },
    { HashStr("sprite/background/background/battle"), { CSD_SCALE } },
    { HashStr("sprite/background/background/fileselect"), { CSD_SCALE } },

    // battledisplay_1p
    { HashStr("sprite/battledisplay_1p/power"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_1p/power_a"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_1p/power_bar_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_1p/itembox_01"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_1p/ring"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_1p/ring_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_1p/enemy"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_1p/bronze_medal"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_1p/gold_medal"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_1p/bar_ue"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_1p/power_bar_effect"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_1p/ring_000_effect"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_1p/custom_gem"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_1p/custom_level1"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_1p/custom_bar_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },

    // battledisplay_2p
    { HashStr("sprite/battledisplay_2p/power"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_2p/power_a"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_2p/power_bar_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_2p/itembox_01"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_2p/ring"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_2p/ring_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_2p/enemy"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_2p/bronze_medal"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_2p/gold_medal"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_2p/bar_ue"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_2p/power_bar_effect"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_2p/ring_000_effect"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_2p/custom_gem"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_2p/custom_level1"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/battledisplay_2p/custom_bar_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },

    // battle_result
    { HashStr("sprite/battle_result/battle_result/congratulations"), { CSD_ALIGN_TOP } },
    { HashStr("sprite/battle_result/battle_result/plate/plate_ue/plate1"), { CSD_ALIGN_TOP | CSD_EXTEND_LEFT } },
    { HashStr("sprite/battle_result/battle_result/plate/plate_ue/plate2"), { CSD_ALIGN_TOP } },
    { HashStr("sprite/battle_result/battle_result/plate/plate_ue/plate3"), { CSD_ALIGN_TOP } },
    { HashStr("sprite/battle_result/battle_result/plate/plate_ue/plate4"), { CSD_ALIGN_TOP | CSD_EXTEND_RIGHT } },
    { HashStr("sprite/battle_result/battle_result/plate/plate_sita/plate5"), { CSD_ALIGN_BOTTOM | CSD_EXTEND_LEFT } },
    { HashStr("sprite/battle_result/battle_result/plate/plate_sita/plate6"), { CSD_ALIGN_BOTTOM } },
    { HashStr("sprite/battle_result/battle_result/plate/plate_sita/plate7"), { CSD_ALIGN_BOTTOM } },
    { HashStr("sprite/battle_result/battle_result/plate/plate_sita/plate8"), { CSD_ALIGN_BOTTOM | CSD_EXTEND_RIGHT } },

    // black_out
    { HashStr("sprite/black_out/black_out"), { CSD_STRETCH } },

    // bossname
    { HashStr("sprite/bossname/egg_cerberus/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("event/e1071/egg_cerberus/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("event/e1081/egg_cerberus/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/bossname/egg_genesis/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("event/e1091/egg_genesis/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/bossname/egg_wyvern/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("event/e1111/egg_wyvern/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/bossname/egg_wyvern/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("event/e1111/egg_wyvern/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/bossname/iblis/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("event/e1001/iblis/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("event/e1011/iblis/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("event/e1031/iblis/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/bossname/mephiles/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("event/e1041/mephiles/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("event/e1061/mephiles/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/bossname/shadow_the_hedgehog/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("event/e1161/shadow_the_hedgehog/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/bossname/silver_the_hedgehog/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("event/e1151/silver_the_hedgehog/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/bossname/solaris/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("event/e1121/solaris/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/bossname/sonic_the_hedgehog/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("event/e1141/sonic_the_hedgehog/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },

    // button_window
    { HashStr("sprite/button_window/button_window/Scene_0000/Null_0000/Cast_0001"), { CSD_SCALE } },
    { HashStr("sprite/button_window/button_window/Scene_0000/Null_0000/Cast_0002"), { CSD_SCALE | CSD_EXTEND_RIGHT } },

    // cri_logo
    { HashStr("sprite/logo/cri_logo/Scene_0000/Null_0002/bg"), { CSD_STRETCH } },
    { HashStr("sprite/logo/cri_logo/Scene_0000/Null_0002/criware"), { CSD_CRI_LOGO | CSD_SCALE } },

    // gadget_ber
    { HashStr("sprite/gadget_ber/gadget_bar/gadgetbar"), { CSD_ALIGN_BOTTOM_RIGHT } },
    { HashStr("sprite/gadget_ber/gadget_bar/gadgetbar_anime"), { CSD_ALIGN_BOTTOM_RIGHT } },
    { HashStr("sprite/gadget_ber/gadget_bar/gadgetbar_ue"), { CSD_ALIGN_BOTTOM_RIGHT } },
    { HashStr("sprite/gadget_ber/gadget_bar/icon_text"), { CSD_ALIGN_BOTTOM_RIGHT } },

    // goldmedal
    { HashStr("sprite/goldmedal/goldmedal/ranking"), { CSD_SCALE } },
    { HashStr("sprite/goldmedal/goldmedal/goldmedal"), { CSD_SCALE } },
    { HashStr("sprite/goldmedal/goldmedal/charaselect_cursor"), { CSD_SCALE } },
    { HashStr("sprite/goldmedal/goldmedal/charaselect_cursor2"), { CSD_SCALE } },
    { HashStr("sprite/goldmedal/goldmedal/board_cursor_sita"), { CSD_SCALE } },
    { HashStr("sprite/goldmedal/goldmedal/board_cursor_ue"), { CSD_SCALE } },
    { HashStr("sprite/goldmedal/goldmedal/total_medal"), { CSD_SCALE } },
    { HashStr("sprite/goldmedal/goldmedal/sonic"), { CSD_SCALE } },
    { HashStr("sprite/goldmedal/goldmedal/shadow"), { CSD_SCALE } },
    { HashStr("sprite/goldmedal/goldmedal/silver"), { CSD_SCALE } },
    { HashStr("sprite/goldmedal/goldmedal/last"), { CSD_SCALE } },

    // tips_sh_x
    { HashStr("sprite/interlude/tips_x/tips_sh_x/Scene_0000"), { CSD_SCALE } },

    // tips_si_x
    { HashStr("sprite/interlude/tips_x/tips_si_x/Scene_0000"), { CSD_SCALE } },

    // tips_so_x
    { HashStr("sprite/interlude/tips_x/tips_so_x/Scene_0000"), { CSD_SCALE } },

    // loading
    { HashStr("sprite/loading/loading/Scene_0000/Loading"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_MODIFIER_NARROW_ONLY } },
    { HashStr("sprite/loading/loading/Scene_0000/Loading_02"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_MODIFIER_NARROW_ONLY } },
    { HashStr("sprite/loading/loading/Scene_0000/arrow_01"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_MODIFIER_NARROW_ONLY } },
    { HashStr("sprite/loading/loading/Scene_0000/arrow_02"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_MODIFIER_NARROW_ONLY } },
    { HashStr("sprite/loading/loading/Scene_0000/arrow_03"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_MODIFIER_NARROW_ONLY } },

    // maindisplay
    { HashStr("sprite/maindisplay/power"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/custom_bar_anime"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/power_a"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/power_bar_anime"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/itembox_01"), { CSD_ALIGN_BOTTOM | CSD_SCALE } },
    { HashStr("sprite/maindisplay/score"), { CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/life_ber_anime"), { CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/life"), { CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/time"), { CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/ring"), { CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/ring_anime"), { CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/bar_ue"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/power_bar_effect"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/ring_000_effect"), { CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/boss_gauge"), { CSD_ALIGN_TOP_RIGHT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/boss_gauge_anime"), { CSD_ALIGN_TOP_RIGHT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/item_ber_anime"), { CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/item"), { CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/custom_gem"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/maindisplay/custom_level"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },

    // main_menu
    { HashStr("sprite/main_menu/main_menu_cursor"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/main_menu_cursor2"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/main_menu_cursor3"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/eposodeselect"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/episodeselect_cursor1"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/episodeselect_cursor2"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/savedate/savedata/Null_1074/sita3"), { CSD_SCALE | CSD_EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/savedate/savedata/Null_1074/sita5"), { CSD_SCALE | CSD_EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/savedate/savedata/Null_1074/sita6"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/savedate/savedata/Null_1074/sita7"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/savedate/savedata/Null_1074/sita8"), { CSD_SCALE | CSD_EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/savedate/savedata/Null_1074/sita9"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/savedate/savedata/goldmedal"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/savedate/savedata/zanki_3"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/savedate/savedata/zanki_2"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/savedate/savedata/zanki"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/savedate/savedata/ring"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/character_choice"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/character_cursor1"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/character_cursor2"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/stage_plate/stage_plate"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/stage_plate/stage_plate/stage_plate2"), { CSD_SCALE | CSD_EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/stage_plate/stage_plate/Cast_1337"), { CSD_SCALE | CSD_EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/stage_plate/stage_plate/Cast_1338"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/stage_plate/stage_plate/Cast_1339"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/stage_plate/stage_plate/Cast_1340"), { CSD_SCALE | CSD_EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/mission_plate/mission_plate"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/mission_plate/mission_plate/mission_plate2"), { CSD_SCALE | CSD_EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/mission_plate/mission_plate/Cast_1332"), { CSD_SCALE | CSD_EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/mission_plate/mission_plate/Cast_1334"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/mission_plate/mission_plate/Cast_1335"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/mission_plate/mission_plate/Cast_1336"), { CSD_SCALE | CSD_EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/stage_cursor"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/mission_cursor"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/stage_cursor2"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/text"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/text_cover/Null_0290/cover_l"), { CSD_SCALE | CSD_UV_MODIFIER | CSD_REPEAT_LEFT | CSD_REPEAT_FLIP_HORIZONTAL | CSD_REPEAT_EXTEND | CSD_REPEAT_UV_MODIFIER, { 0.0015f, 0, 0.0015f, 0, 0, 0, 0, 0 }, {}, { 0, 0, 0, 0, -0.05f, 0, -0.05f, 0 } } },
    { HashStr("sprite/main_menu/text_cover/Null_0290/cover_c"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/text_cover/Null_0290/vocer_r"), { CSD_SCALE | CSD_UV_MODIFIER | CSD_REPEAT_RIGHT | CSD_REPEAT_FLIP_HORIZONTAL | CSD_REPEAT_EXTEND | CSD_REPEAT_UV_MODIFIER, { 0, 0, 0, 0, 0.0015f, 0, 0.0015f, 0 }, {}, { -0.05f, 0, -0.05f, 0, 0, 0, 0, 0 } } },
    { HashStr("sprite/main_menu/mission_text/rank"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/mission_text/item_icon"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/mission_text/m_ring_icon"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/main_menu_parts/Null_0218/Cast_0221"), { CSD_MAIN_MENU_PARTS_CAST_0221 | CSD_SCALE | CSD_EXTEND_RIGHT, { 0, 0.005f, 0, 0, 0, 0.005f, 0, 0 }, {}, { 0, 0, 0, -0.0175f, 0, 0, 0, -0.0175f } } },
    { HashStr("sprite/main_menu/main_menu_parts/Null_0218/Cast_0222"), { CSD_MAIN_MENU_PARTS_CAST_0222 | CSD_SCALE | CSD_UV_MODIFIER, { 0.0015f, 0, 0.0015f, 0, 0, 0, 0, 0 } } },
    { HashStr("sprite/main_menu/main_menu_parts/Null_0960/Cast_0964"), { CSD_SCALE | CSD_EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/main_menu_parts/Null_0960/Cast_0965"), { CSD_SCALE | CSD_REPEAT_LEFT | CSD_REPEAT_FLIP_HORIZONTAL | CSD_REPEAT_EXTEND | CSD_REPEAT_UV_MODIFIER, {}, {}, { 0, 0, 0, 0, -0.5f, 0, -0.5f, 0 } } },
    { HashStr("sprite/main_menu/main_menu_parts/Null_0960/Cast_0966"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/main_menu_parts/Null_0960/Cast_0966/Cast_0967"), { CSD_SCALE } },
    { HashStr("sprite/main_menu/main_menu_parts/Null_0224/Cast_0226"), { CSD_MAIN_MENU_PARTS_CAST_0226 | CSD_SCALE } },
    { HashStr("sprite/main_menu/main_menu_parts/Null_0224/Cast_0227"), { CSD_MAIN_MENU_PARTS_CAST_0227 | CSD_SCALE } },
    { HashStr("sprite/main_menu/titlebar_effect"), { CSD_SCALE } },

    // map_twn
    { HashStr("sprite/interlude/map_twn/map_twn/Scene_0000"), { CSD_SCALE } },

    // pausemenu
    { HashStr("sprite/pausemenu/pausemenu/pause_menu"), { CSD_SCALE } },
    { HashStr("sprite/pausemenu/pausemenu/pause_menu_cursor"), { CSD_SCALE } },
    { HashStr("sprite/pausemenu/pausemenu/mission"), { CSD_SCALE } },

    // radarmap_cover
    { HashStr("sprite/radarmap_cover/radarmap_cover/Scene_0000"), { CSD_RADARMAP | CSD_ALIGN_TOP_RIGHT | CSD_SCALE } },

    // result
    { HashStr("sprite/result/result/title_plate/plate_ue"), { CSD_ALIGN_TOP | CSD_SCALE } },
    { HashStr("sprite/result/result/title_plate/plate_sita"), { CSD_ALIGN_BOTTOM | CSD_SCALE } },
    { HashStr("sprite/result/result/title_plate/result_title/result_title_ob"), { CSD_ALIGN_TOP | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/score/score_plate_kage"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/score/score_plate"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/score/score_plate/score_text"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/score/score_plate/score_plate_title"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/time/time_plate_kage"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/time/time_plate"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/time/time_plate/time_text"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/time/time_plate/time_plate_title"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/ring/ring_plare_kage"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/ring/ring_plate"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/ring/ring_plate/ring_text"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/ring/ring_plate/ring_title"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/time_bonus/time_bonus_kage"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/time_bonus/time_bonus_plate"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/time_bonus/time_bonus_plate/timebonus_text"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/time_bonus/time_bonus_plate/time_bonus_plate_title"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/ring_bonus/ring_bonus_plate_kage"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/ring_bonus/ring_bonus_plate"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/ring_bonus/ring_bonus_plate/ringbonus_text"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/ring_bonus/ring_bonus_plate/ring_bonus_plate_title"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/amiplate/ami_plate_kage"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/result/parts_anime_result/amiplate/ami_plate"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/town_score_t/score_plate_kage_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/town_score_t/score_plate_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/town_score_t/score_plate_t/score_text_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/town_score_t/score_plate_t/score_plate_title_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/time_t/time_plate_kage_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/time_t/time_plate_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/time_t/time_plate_t/time_text_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/time_t/time_plate_t/time_plate_title_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/ring_t/ring_plare_kage_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/ring_t/ring_plate_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/ring_t/ring_plate_t/ring_text_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/ring_t/ring_plate_t/ring_title_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/time_bonus_t/time_bonus_kage_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/time_bonus_t/time_bonus_plate_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/time_bonus_t/time_bonus_plate_t/timebonus_text_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/time_bonus_t/time_bonus_plate_t/time_bonus_plate_title_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/ring_bonus_t/ring_bonus_plate_kage_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/ring_bonus_t/ring_bonus_plate_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/ring_bonus_t/ring_bonus_plate_t/ringbonus_text_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/ring_bonus_t/ring_bonus_plate_t/ring_bonus_plate_title_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/record_t/record_plate_kage_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/record_t/record_bonus_plate_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/record_t/record_bonus_plate_t/record_text_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/record_t/record_bonus_plate_t/record_plate_title_t"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/record_t/item_icon"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/town_mission/parts_anime_result_t/amiplate_t"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_s/list_plate_kage01"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_s/list_plate01"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_s/list_plate01/list_text01"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_s/list_plate01/list_text01/nyoro01"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_s/list_plate01/ring01"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_s/list_plate01/ring01/ring01_b"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_s/list_plate01/bonus_text01"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_s/list_plate01/rank_list_s_kage"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_s/list_plate01/rank_list_s"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_a/list_plate_kage02"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_a/list_plate02"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_a/list_plate02/list_text02"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_a/list_plate02/list_text02/nyoro02"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_a/list_plate02/ring02"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_a/list_plate02/ring02/ring02_b"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_a/list_plate02/bonus_text02"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_a/list_plate02/rank_list_a_kage"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_a/list_plate02/rank_list_a"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_b/list_plate_kage03"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_b/list_plate03"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_b/list_plate03/list_text03"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_b/list_plate03/list_text03/nyoro03"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_b/list_plate03/ring03"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_b/list_plate03/ring03/ring03_b"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_b/list_plate03/bonus_text03"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_b/list_plate03/rank_list_b_kage"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_b/list_plate03/rank_list_b"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_c/list_plate_kage04"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_c/list_plate04"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_c/list_plate04/list_text04"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_c/list_plate04/list_text04/nyoro04"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_c/list_plate04/ring04"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_c/list_plate04/ring04/ring04_b"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_c/list_plate04/bonus_text04"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_c/rank_list_c_kage"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_c/rank_list_c"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_d/list_plate_kage05"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_d/list_plate05"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_d/list_plate05/list_text05"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_d/list_plate05/list_text05/nyoro05"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_d/list_plate05/ring05"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_d/list_plate05/ring05/ring05_b"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_d/list_plate05/bonus_text05"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_d/list_plate05/rank_list_d_kage"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/bonus_list/rank_d/list_plate05/rank_list_d"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/amiplate/ami_plate_kage"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/amiplate/ami_plate"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/rank_bonus/rank_bonus_plate_kage"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/rank_bonus/rank_bonus_plate"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/rank_bonus/rank_bonus_plate/rank_bonus_plate_text"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/rank_bonus/rank_bonus_plate/rank_bonus_plate_title"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/rank_bonus/rank_bonus_ring"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/rank_bonus/rank_bonus_ring/rank_bonus_ring_b"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/total_ring/total_ring_plate_kage"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/total_ring/total_ring_plate"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/total_ring/total_ring_plate/total_ring_plate_title"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/TotalRing_Goldfont/total_ring_text"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/TotalRing_Goldfont/ring_l"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/bonus/parts_anime_bonus/TotalRing_Goldfont/ring_l_b"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/totalscore_rank/parts_anime/total_score/total_score_plate_kage"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/totalscore_rank/parts_anime/total_score/total_score_plate"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/totalscore_rank/parts_anime/total_score/total_score_plate/totalscore_text"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/totalscore_rank/parts_anime/total_score/total_score_plate/total_score_plate_title"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/totalscore_rank/parts_anime/rank/rank_plate_kage"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/totalscore_rank/parts_anime/rank/rank_plate"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/totalscore_rank/parts_anime/rank/rank_plate/rank_plate_title"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/rank_anime"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/result/result/score_newrecord"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/time_newrecord"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/ring_newrecord"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/timebonus_newrecord"), { CSD_ALIGN_LEFT | CSD_SCALE } },
    { HashStr("sprite/result/result/ringbonus_newrecord"), { CSD_ALIGN_LEFT | CSD_SCALE } },

    // result WIP ultrawide adjustments
    // { HashStr("sprite/result/result/title_plate/plate_ue/plate_ue_kage_l"), { CSD_ALIGN_TOP | CSD_SCALE | CSD_REPEAT_FLIP_HORIZONTAL | CSD_UV_MODIFIER | CSD_REPEAT_LEFT | CSD_REPEAT_EXTEND | CSD_REPEAT_UV_MODIFIER, { 0.015f, 0, 0.015f, 0, 0, 0, 0, 0 }, {}, { 0, 0, 0, 0, -0.6f, 0, -0.6f, 0 } } },
    // { HashStr("sprite/result/result/title_plate/plate_ue/plate_ue_kage_r"), { CSD_ALIGN_TOP | CSD_SCALE | CSD_REPEAT_FLIP_HORIZONTAL | CSD_UV_MODIFIER | CSD_REPEAT_RIGHT | CSD_REPEAT_EXTEND | CSD_REPEAT_UV_MODIFIER, { 0, 0, 0, 0, 0.015f, 0, 0.015f, 0 }, {}, { -0.6f, 0, -0.6f, 0, 0, 0, 0, 0 } } },
    // { HashStr("sprite/result/result/title_plate/plate_ue/plate_ue_plate_l"), { CSD_ALIGN_TOP | CSD_SCALE | CSD_REPEAT_FLIP_HORIZONTAL | CSD_UV_MODIFIER | CSD_REPEAT_LEFT | CSD_REPEAT_EXTEND | CSD_REPEAT_UV_MODIFIER, { 0.015f, 0, 0.015f, 0, 0, 0, 0, 0 }, {}, { 0, 0, 0, 0, -0.6f, 0, -0.6f, 0 } } },
    // { HashStr("sprite/result/result/title_plate/plate_ue/plate_ue_plate_r"), { CSD_ALIGN_TOP | CSD_SCALE | CSD_REPEAT_FLIP_HORIZONTAL | CSD_UV_MODIFIER | CSD_REPEAT_RIGHT | CSD_REPEAT_EXTEND | CSD_REPEAT_UV_MODIFIER, { 0, 0, 0, 0, -0.015f, 0, -0.015f, 0 }, {}, { 0.6f, 0, 0.6f, 0, 0, 0, 0, 0 } } },
    // { HashStr("sprite/result/result/title_plate/plate_sita/plate_sita_kage_l"), { CSD_ALIGN_BOTTOM | CSD_REPEAT_FLIP_HORIZONTAL | CSD_UV_MODIFIER | CSD_REPEAT_RIGHT | CSD_REPEAT_EXTEND | CSD_REPEAT_UV_MODIFIER, { 0, 0, 0, 0, -0.08f, 0, -0.08f, 0 }, {}, { 0.6f, 0, 0.6f, 0, 0, 0, 0, 0 } } },    // TODO: this crashes the game.
    // { HashStr("sprite/result/result/title_plate/plate_sita/plate_sita_kage_r"), { CSD_ALIGN_BOTTOM | CSD_REPEAT_FLIP_HORIZONTAL | CSD_UV_MODIFIER | CSD_REPEAT_LEFT | CSD_REPEAT_EXTEND | CSD_REPEAT_UV_MODIFIER, { 0.015f, 0, 0.015f, 0, 0, 0, 0, 0 }, {}, { 0, 0, 0, 0, -0.6f, 0, -0.6f, 0 } } },   // TODO: this crashes the game.
    // { HashStr("sprite/result/result/title_plate/plate_sita/plate_sita_plate_l"), { CSD_ALIGN_BOTTOM | CSD_REPEAT_FLIP_HORIZONTAL | CSD_UV_MODIFIER | CSD_REPEAT_RIGHT | CSD_REPEAT_EXTEND | CSD_REPEAT_UV_MODIFIER, { 0, 0, 0, 0, -0.015f, 0, -0.015f, 0 }, {}, { 0.6f, 0, 0.6f, 0, 0, 0, 0, 0 } } }, // TODO: this crashes the game.
    // { HashStr("sprite/result/result/title_plate/plate_sita/plate_sita_plate_r"), { CSD_ALIGN_BOTTOM | CSD_REPEAT_FLIP_HORIZONTAL | CSD_UV_MODIFIER | CSD_REPEAT_LEFT | CSD_REPEAT_EXTEND | CSD_REPEAT_UV_MODIFIER, { 0.015f, 0, 0.015f, 0, 0, 0, 0, 0 }, {}, { 0, 0, 0, 0, -0.6f, 0, -0.6f, 0 } } },  // TODO: this crashes the game.
    // { HashStr("sprite/result/result/result/parts_anime_result/amiplate/ami_plate"), { CSD_REPEAT_RIGHT | CSD_REPEAT_FLIP_HORIZONTAL | CSD_REPEAT_FLIP_VERTICAL | CSD_UV_MODIFIER | CSD_REPEAT_UV_MODIFIER, { 0, 0, 0, 0, -0.025f, 0, -0.025f, 0 }, {}, { -0.01285f, 0.002f, -0.01285f, 0.002f, -0.01285f, 0.002f, -0.01285f, 0.002f } } },
    // { HashStr("sprite/result/result/town_mission/parts_anime_result_t/amiplate_t/ami_plate_t"), { CSD_REPEAT_RIGHT | CSD_REPEAT_FLIP_HORIZONTAL | CSD_REPEAT_FLIP_VERTICAL | CSD_UV_MODIFIER | CSD_REPEAT_UV_MODIFIER, { 0, 0, 0, 0, -0.025f, 0, -0.025f, 0 }, {}, { -0.01285f, 0.002f, -0.01285f, 0.002f, -0.01285f, 0.002f, -0.01285f, 0.002f } } },
    // { HashStr("sprite/result/result/bonus/parts_anime_bonus/amiplate/ami_plate"), { CSD_REPEAT_LEFT | CSD_REPEAT_FLIP_HORIZONTAL | CSD_REPEAT_FLIP_VERTICAL | CSD_UV_MODIFIER | CSD_REPEAT_UV_MODIFIER, { 0.025f, 0, 0.025f, 0, 0, 0, 0, 0 }, {}, { 0.027f, 0.002f, 0.027f, 0.002f, 0.027f, 0.002f, 0.027f, 0.002f } } },

    // result amiplate fade (needs special case for 16:9 or narrower)
    // { HashStr("sprite/result/result/result/parts_anime_result/amiplate/ami_plate"), { CSD_COLOUR_MODIFIER, {}, { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFF00, 0xFFFFFF00 }, } },
    // { HashStr("sprite/result/result/town_mission/parts_anime_result_t/amiplate_t/ami_plate_t"), { CSD_COLOUR_MODIFIER, {}, { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFF00, 0xFFFFFF00 } } },
    // { HashStr("sprite/result/result/bonus/parts_anime_bonus/amiplate/ami_plate"), { CSD_COLOUR_MODIFIER, {}, { 0xFFFFFF00, 0xFFFFFF00, 0xFFFFFFFF, 0xFFFFFFFF } } },

    // sonicteam_logo
    { HashStr("sprite/logo/sonicteam_logo/sonicteam"), { CSD_SCALE } },

    // stagetitle
    { HashStr("sprite/interlude/stagetitle/stage_title_aqa/Scene_0000"), { CSD_SCALE } },
    { HashStr("sprite/interlude/stagetitle/stage_title_cmn/Scene_0000"), { CSD_SCALE } },
    { HashStr("sprite/interlude/stagetitle/stage_title_csc/Scene_0000"), { CSD_SCALE } },
    { HashStr("sprite/interlude/stagetitle/stage_title_dtd/Scene_0000"), { CSD_SCALE } },
    { HashStr("sprite/interlude/stagetitle/stage_title_end/Scene_0000"), { CSD_SCALE } },
    { HashStr("sprite/interlude/stagetitle/stage_title_flc/Scene_0000"), { CSD_SCALE } },
    { HashStr("sprite/interlude/stagetitle/stage_title_kdv/Scene_0000"), { CSD_SCALE } },
    { HashStr("sprite/interlude/stagetitle/stage_title_rct/Scene_0000"), { CSD_SCALE } },
    { HashStr("sprite/interlude/stagetitle/stage_title_tpj/Scene_0000"), { CSD_SCALE } },
    { HashStr("sprite/interlude/stagetitle/stage_title_twn_a/Scene_0000"), { CSD_SCALE } },
    { HashStr("sprite/interlude/stagetitle/stage_title_twn_b/Scene_0000"), { CSD_SCALE } },
    { HashStr("sprite/interlude/stagetitle/stage_title_twn_c/Scene_0000"), { CSD_SCALE } },
    { HashStr("sprite/interlude/stagetitle/stage_title_wap/Scene_0000"), { CSD_SCALE } },
    { HashStr("sprite/interlude/stagetitle/stage_title_wvo/Scene_0000"), { CSD_SCALE } },

    // tag_character
    { HashStr("sprite/tag_character/tag_character/1p_tug/1p_tug/1p_tug1"), { CSD_SCALE | CSD_REPEAT_LEFT | CSD_REPEAT_FLIP_HORIZONTAL | CSD_REPEAT_EXTEND | CSD_REPEAT_UV_MODIFIER | CSD_REPEAT_COLOUR_MODIFIER, {}, {}, { 0, 0, 0, 0, -0.5f, 0, -0.5f, 0 }, { 0xFFFFFF50, 0xFFFFFF50, 0xFFFFFF50, 0xFFFFFF50 } } },
    { HashStr("sprite/tag_character/tag_character/1p_tug/1p_tug/1p_tug2"), { CSD_SCALE } },
    { HashStr("sprite/tag_character/tag_character/2p_tug/2p_tug/2p_tug1"), { CSD_SCALE | CSD_REPEAT_RIGHT | CSD_REPEAT_FLIP_HORIZONTAL | CSD_REPEAT_EXTEND | CSD_REPEAT_UV_MODIFIER | CSD_REPEAT_COLOUR_MODIFIER, {}, {}, { 0.5f, 0, 0.5f, 0, 0, 0, 0, 0 }, { 0xFFFFFF50, 0xFFFFFF50, 0xFFFFFF50, 0xFFFFFF50 } } },
    { HashStr("sprite/tag_character/tag_character/2p_tug/2p_tug/2p_tug2"), { CSD_SCALE } },
    { HashStr("sprite/tag_character/tag_character/1p_cursor"), { CSD_SCALE } },
    { HashStr("sprite/tag_character/tag_character/2p_cursor"), { CSD_SCALE } },
    { HashStr("sprite/tag_character/tag_character/1p_name"), { CSD_SCALE } },
    { HashStr("sprite/tag_character/tag_character/2p_name"), { CSD_SCALE } },
    { HashStr("sprite/tag_character/tag_character/stage_window"), { CSD_SCALE } },
    { HashStr("sprite/tag_character/tag_character/stage"), { CSD_SCALE } },
    { HashStr("sprite/tag_character/tag_character/controller_1p"), { CSD_SCALE } },
    { HashStr("sprite/tag_character/tag_character/controller_2p"), { CSD_SCALE } },
    { HashStr("sprite/tag_character/tag_character/entry"), { CSD_SCALE } },

    // tagdisplay_1p
    { HashStr("sprite/tagdisplay_1p/power"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/power_a"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/power_bar_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/itembox_01"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/score"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/time"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/ring"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/ring_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/bar_ue"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/life_bar_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/life"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/item_ber_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/item"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/power_bar_effect"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/ring_000_effect"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/custom_gem"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/custom_level1"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_1p/custom_bar_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },

    // tagdisplay_2p
    { HashStr("sprite/tagdisplay_2p/power"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/power_a"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/power_bar_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/itembox_01"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/score"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/time"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/ring"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/ring_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/bar_ue"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/life_bar_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/life"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/item_ber_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/item"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/power_bar_effect"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/ring_000_effect"), { CSD_MULTIPLAYER | CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/custom_gem"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/custom_level1"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },
    { HashStr("sprite/tagdisplay_2p/custom_bar_anime"), { CSD_MULTIPLAYER | CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE } },

    // talkwindow
    { HashStr("sprite/talkwindow/talkwindow/window"), { CSD_ALIGN_BOTTOM | CSD_SCALE } },
    { HashStr("sprite/talkwindow/talkwindow/nafuda"), { CSD_ALIGN_BOTTOM | CSD_SCALE } },
    { HashStr("sprite/talkwindow/talkwindow/Scene_0021"), { CSD_ALIGN_BOTTOM | CSD_SCALE } },

    // title
    { HashStr("sprite/title/title/Scene_Title/Logo_add"), { CSD_SCALE } },
    { HashStr("sprite/title/title/Scene_Title/Logo"), { CSD_SCALE } },
    { HashStr("sprite/title/title/Scene_Title/copyright"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_MODIFIER_NARROW_ONLY } },

    // titleloop_sth
    { HashStr("sprite/logo/titleloop_sth/Scene_0000"), { CSD_ALIGN_BOTTOM_RIGHT | CSD_SCALE | CSD_MOVIE | CSD_MODIFIER_NARROW_ONLY } },

    // towndisplay
    { HashStr("sprite/towndisplay/ring"), { CSD_ALIGN_TOP_LEFT | CSD_SCALE } },
    { HashStr("sprite/towndisplay/ring_anime"), { CSD_ALIGN_TOP_LEFT | CSD_SCALE } },

    // trickpoint
    { HashStr("sprite/trickpoint/trickpoint/Scene_0000"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/trickpoint/trickpoint/Scene_0001"), { CSD_ALIGN_RIGHT | CSD_SCALE } },
    { HashStr("sprite/trickpoint/trickpoint/score"), { CSD_ALIGN_RIGHT | CSD_SCALE } },

    // windowtest
    { HashStr("sprite/windowtest/Scene_0000"), { CSD_SCALE } },
    { HashStr("sprite/windowtest/okuribotan"), { CSD_SCALE } },
    { HashStr("sprite/windowtest/cursor"), { CSD_SCALE } },
};

std::optional<CsdModifier> FindCsdModifier(uint32_t data)
{
    XXH64_hash_t path;
    {
        std::lock_guard lock(g_pathMutex);

        auto findResult = g_paths.find(g_memory.Translate(data));

        if (findResult == g_paths.end())
            return {};

        path = findResult->second;
    }

    auto findResult = g_csdModifiers.find(path);

    if (findResult != g_csdModifiers.end())
        return findResult->second;

    return {};
}

// -------------- TEXT MODIFIERS -------------- //

const xxHashMap<TextFontPictureModifier> g_pftModifierXenon =
{
    { HashStr("button_a"), { 0, 0, 28, 28 } },
    { HashStr("button_b"), { 28, 0, 28, 28 } },
    { HashStr("button_x"), { 56, 0, 28, 28 } },
    { HashStr("button_y"), { 84, 0, 28, 28 } },
    { HashStr("button_lb"), { 112, 0, 53, 28 } },
    { HashStr("button_lt"), { 56, 28, 55, 28 } },
    { HashStr("button_rb"), { 168, 0, 53, 28 } },
    { HashStr("button_rt"), { 0, 28, 55, 28 } },
    { HashStr("button_start"), { 112, 28, 28, 28 } },
    { HashStr("button_back"), { 140, 28, 28, 28 } }
};

const xxHashMap<TextFontPictureModifier> g_pftModifierPS3 =
{
    { HashStr("button_a"), { 0, 56, 28, 28 } },
    { HashStr("button_b"), { 28, 56, 28, 28 } },
    { HashStr("button_x"), { 56, 56, 28, 28 } },
    { HashStr("button_y"), { 84, 56, 28, 28 } },
    { HashStr("button_lb"), { 112, 56, 48, 28 } },
    { HashStr("button_lt"), { 168, 56, 48, 28 } },
    { HashStr("button_rb"), { 0, 84, 48, 28 } },
    { HashStr("button_rt"), { 56, 84, 48, 28 } },
    { HashStr("button_start"), { 140, 84, 28, 28 } },
    { HashStr("button_back"), { 112, 84, 28, 28 } }
};

TextFontPictureModifier FindFontPictureModifier(xxHashMap<TextFontPictureModifier> pftModifier, std::string_view& name)
{
    auto findResult = pftModifier.find(HashStr(name));

    if (findResult != pftModifier.end())
        return findResult->second;

    return {};
}

// ------------- MOVIE MODIFIERS -------------- //

const xxHashMap<MovieModifier> g_movieModifiers =
{
    { HashStr("sound\\title_loop_GBn.wmv"), { MOVIE_CROP_NARROW } }
};

MovieModifier FindMovieModifier(XXH64_hash_t nameHash)
{
    auto findResult = g_movieModifiers.find(nameHash);

    if (findResult != g_movieModifiers.end())
        return findResult->second;

    return {};
}
