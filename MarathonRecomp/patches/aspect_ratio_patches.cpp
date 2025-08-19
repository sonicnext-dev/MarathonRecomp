#include "aspect_ratio_patches.h"
#include <api/Marathon.h>
#include <gpu/video.h>
#include <hid/hid.h>
#include <patches/hook_event.h>
#include <patches/loading_patches.h>
#include <ui/black_bar.h>
#include <ui/game_window.h>
#include <ui/imgui_utils.h>
#include <user/config.h>
#include <app.h>

// #define CORNER_DEBUG

static Mutex g_pathMutex;
static std::map<const void*, XXH64_hash_t> g_paths;

static std::optional<CsdModifier> g_sceneModifier;
static std::optional<CsdModifier> g_castNodeModifier;
static std::optional<CsdModifier> g_castModifier;

static Sonicteam::TextFontPicture* g_pTextFontPicture;

static float g_corners[8];
static bool g_cornerExtract;

static float g_radarMapX;
static float g_radarMapY;
static float g_radarMapCoverWidth;
static float g_radarMapCoverHeight;

// Explicit translations don't get affected by gameplay UI downscaling.
static float g_scenePositionX;
static float g_scenePositionY;

static class LoadingPillarboxEvent : public HookEvent
{
public:
    void Update(float deltaTime) override
    {
        BlackBar::g_isPillarbox = true;
    }
}
g_loadingPillarboxEvent{};

static float ComputeScale(float aspectRatio)
{
    return ((aspectRatio * 720.0f) / 1280.0f) / sqrt((aspectRatio * 720.0f) / 1280.0f);
}

void AspectRatioPatches::Init()
{
    LoadingPatches::Events.push_back(&g_loadingPillarboxEvent);
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

    g_aspectRatioNarrowScale = std::clamp((g_aspectRatio - NARROW_ASPECT_RATIO) / (WIDE_ASPECT_RATIO - NARROW_ASPECT_RATIO), 0.0f, 1.0f);
}

static void EmplacePath(const void* key, const std::string_view& value)
{
    std::lock_guard lock(g_pathMutex);
    g_paths.emplace(key, HashStr(value));
}

static void TraverseCast(Chao::CSD::Scene* scene, uint32_t castNodeIndex, Chao::CSD::CastNode* castNode, uint32_t castIndex, const std::string& parentPath)
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

static void TraverseScene(Chao::CSD::Scene* scene, std::string path)
{
    EmplacePath(scene, path);
    path += "/";

    for (size_t i = 0; i < scene->CastNodeCount; i++)
    {
        auto& castNode = scene->pCastNodes[i];
        TraverseCast(scene, i, &castNode, castNode.RootCastIndex, path);
    }
}

static void TraverseSceneNode(Chao::CSD::SceneNode* sceneNode, std::string path)
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

static std::optional<CsdModifier> FindCsdModifier(uint32_t data)
{
    XXH64_hash_t path;
    {
        std::lock_guard lock(g_pathMutex);

        auto findResult = g_paths.find(g_memory.Translate(data));
        if (findResult == g_paths.end())
            return {};

        path = findResult->second;
    }

    auto findResult = g_modifiers.find(path);
    if (findResult != g_modifiers.end())
        return findResult->second;

    return {};
}

// Sonicteam::CsdResource::MakeResource
PPC_FUNC_IMPL(__imp__sub_82617570);
PPC_FUNC(sub_82617570)
{
    auto pName = reinterpret_cast<const char*>(base + PPC_LOAD_U32(ctx.r4.u32 + 4));

    __imp__sub_82617570(ctx, base);

    if (!ctx.r3.u32)
        return;

    auto ppCsdObject = PPC_LOAD_U32(ctx.r3.u32);

    if (!ppCsdObject)
        return;

    auto pCsdObject = reinterpret_cast<Sonicteam::CsdObject*>(base + ppCsdObject);

    if (!pCsdObject || !pCsdObject->pCsdProject)
        return;

    LOGFN_UTILITY("CSD loaded: {} (0x{:08X})", pName, (uint64_t)pCsdObject->pCsdProject.get());

    TraverseSceneNode(pCsdObject->pCsdProject->m_pResource->pRootNode, pName);
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

// Chao::CSD::CScene::Render
PPC_FUNC_IMPL(__imp__sub_825CB378);
PPC_FUNC(sub_825CB378)
{
    g_scenePositionX = 0.0f;
    g_scenePositionY = 0.0f;

    uint32_t motionPattern = PPC_LOAD_U32(ctx.r3.u32 + 0x70);
    if (motionPattern != NULL)
    {
        uint32_t member = PPC_LOAD_U32(motionPattern + 8);
        if (member != NULL)
        {
            uint32_t x = PPC_LOAD_U32(member + 0x2C);
            uint32_t y = PPC_LOAD_U32(member + 0x30);

            g_scenePositionX = 1280.0f * reinterpret_cast<float&>(x);
            g_scenePositionY = 720.0f * reinterpret_cast<float&>(y);
        }
    }

    __imp__sub_825CB378(ctx, base);
}

// Chao::CSD::Scene::Render
PPC_FUNC_IMPL(__imp__sub_828C8F60);
PPC_FUNC(sub_828C8F60)
{
    g_sceneModifier = FindCsdModifier(ctx.r3.u32);

    if (g_sceneModifier.has_value())
    {
        if (g_aspectRatio > WIDE_ASPECT_RATIO && (g_sceneModifier->Flags & (OFFSET_SCALE_LEFT | OFFSET_SCALE_RIGHT | CORNER_EXTRACT)) != 0)
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
            if (g_sceneModifier->cornerMax == FLT_MAX)
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

static void Draw(PPCContext& ctx, uint8_t* base, PPCFunc* original, uint32_t stride)
{
    CsdModifier modifier{};

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

    if ((modifier.Flags & SKIP) != 0)
    {
        return;
    }

    if (g_cornerExtract)
    {
        if ((modifier.Flags & (STORE_LEFT_CORNER | STORE_RIGHT_CORNER)) != 0)
        {
            uint32_t vertexIndex = ((modifier.Flags & STORE_LEFT_CORNER) != 0) ? 0 : 3;
            g_corners[modifier.CornerIndex] = *reinterpret_cast<be<float>*>(base + ctx.r4.u32 + vertexIndex * stride);
        }

        return;
    }

    if (Config::UIAlignmentMode == EUIAlignmentMode::Centre)
    {
        if (g_aspectRatio >= WIDE_ASPECT_RATIO)
            modifier.Flags &= ~(ALIGN_LEFT | ALIGN_RIGHT);
    }

    uint32_t size = ctx.r5.u32 * stride;
    ctx.r1.u32 -= size;

    uint8_t* stack = base + ctx.r1.u32;
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

    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float pivotX = 0.0f;
    float pivotY = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;

    bool needsStretch = g_aspectRatio >= WIDE_ASPECT_RATIO;

    if (needsStretch && (modifier.Flags & STRETCH_HORIZONTAL) != 0)
    {
        scaleX = Video::s_viewportWidth / 1280.0f;
    }
    else
    {
        scaleX = g_aspectRatioScale;

        if (needsStretch && (modifier.Flags & UNSTRETCH_HORIZONTAL) != 0)
        {
            pivotX = getVertex(0)->X;
            offsetX = pivotX * Video::s_viewportWidth / 1280.0f;
        }
        else
        {
            if ((modifier.Flags & ALIGN_RIGHT) != 0)
                offsetX = g_aspectRatioOffsetX * 2.0f;
            else if ((modifier.Flags & ALIGN_LEFT) == 0)
                offsetX = g_aspectRatioOffsetX;

            if ((modifier.Flags & SCALE) != 0)
            {
                scaleX *= g_aspectRatioGameplayScale;
                pivotX = g_scenePositionX;

                if ((modifier.Flags & ALIGN_RIGHT) != 0)
                    offsetX += 1280.0f * (1.0f - g_aspectRatioGameplayScale) * g_aspectRatioScale;
                else if ((modifier.Flags & ALIGN_LEFT) == 0)
                    offsetX += 640.0f * (1.0f - g_aspectRatioGameplayScale) * g_aspectRatioScale;

                offsetX += pivotX * g_aspectRatioScale;
            }
        }
    }

    if ((modifier.Flags & STRETCH_VERTICAL) != 0)
    {
        scaleY = Video::s_viewportHeight / 720.0f;
    }
    else
    {
        scaleY = g_aspectRatioScale;

        if ((modifier.Flags & ALIGN_BOTTOM) != 0)
            offsetY = g_aspectRatioOffsetY * 2.0f;
        else if ((modifier.Flags & ALIGN_TOP) == 0)
            offsetY = g_aspectRatioOffsetY;

        if ((modifier.Flags & SCALE) != 0)
        {
            scaleY *= g_aspectRatioGameplayScale;
            pivotY = g_scenePositionY;

            if ((modifier.Flags & ALIGN_BOTTOM) != 0)
                offsetY += 720.0f * (1.0f - g_aspectRatioGameplayScale) * g_aspectRatioScale;
            else if ((modifier.Flags & ALIGN_TOP) == 0)
                offsetY += 360.0f * (1.0f - g_aspectRatioGameplayScale) * g_aspectRatioScale;

            offsetY += pivotY * g_aspectRatioScale;
        }
    }

    if (g_aspectRatio > WIDE_ASPECT_RATIO)
    {
        CsdModifier offsetScaleModifier{};
        float corner = 0.0f;

        if (g_castModifier.has_value())
        {
            offsetScaleModifier = g_castModifier.value();

            uint32_t vertexIndex = ((offsetScaleModifier.Flags & STORE_LEFT_CORNER) != 0) ? 0 : 3;
            corner = getVertex(vertexIndex)->X;
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
        if ((offsetScaleModifier.Flags & (OFFSET_SCALE_LEFT | OFFSET_SCALE_RIGHT)) != 0 && offsetScaleModifier.cornerMax == FLT_MAX)
            fmt::println("Corner: {}", corner);
#endif

        if ((offsetScaleModifier.Flags & OFFSET_SCALE_LEFT) != 0)
            offsetX *= corner / offsetScaleModifier.CornerMax;
        else if ((offsetScaleModifier.Flags & OFFSET_SCALE_RIGHT) != 0)
            offsetX = Video::s_viewportWidth - (Video::s_viewportWidth - offsetX) * (1280.0f - corner) / (1280.0f - offsetScaleModifier.CornerMax);
    }

    float firstX = 0.0f;
    float firstY = 0.0f;
    float lastX = 0.0f;
    float lastY = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    for (size_t i = 0; i < ctx.r5.u32; i++)
    {
        auto vertex = getVertex(i);

        float x = offsetX + (vertex->X - pivotX) * (scaleX * (1280.0f / Video::s_viewportWidth));
        float y = offsetY + (vertex->Y - pivotY) * (scaleY * (720.0f / Video::s_viewportHeight));

        if ((modifier.Flags & EXTEND_LEFT) != 0 && (i == 0 || i == 1))
        {
            x = std::min(x, 0.0f);
        }
        else if ((modifier.Flags & EXTEND_RIGHT) != 0 && (i == 2 || i == 3))
        {
            x = std::max(x, float(Video::s_viewportWidth));
        }

        switch (i)
        {
            case 0: firstX = x; break;
            case 1: lastY  = y; break;
            case 2: firstY = y; break;
            case 3: lastX  = x; break;
        }

        if ((modifier.Flags & RADARMAP) != 0)
        {
            g_radarMapX = x;
            g_radarMapY = y;
        }

        vertex->X = round(x);
        vertex->Y = round(y);
    }

    width = lastX - firstX;
    height = lastY - firstY;

    if ((modifier.Flags & RADARMAP) != 0)
    {
        g_radarMapCoverWidth = width;
        g_radarMapCoverHeight = height;
    }

    if ((modifier.Flags & PILLARBOX) != 0)
        BlackBar::g_isPillarbox = true;

    if ((modifier.Flags & PROHIBIT_BLACK_BAR) != 0)
        BlackBar::g_isPillarbox = false;

    auto isRepeatLeft = (modifier.Flags & REPEAT_LEFT) != 0;
    auto isRepeatRight = (modifier.Flags & REPEAT_RIGHT) != 0;

    if (isRepeatLeft || isRepeatRight)
    {
        auto r3 = ctx.r3;
        auto r5 = ctx.r5;
        auto vertexIndex = isRepeatLeft ? 2 : 0;
        auto x = getVertex(vertexIndex)->X;

        while (isRepeatLeft ? x > 0.0f : x < float(Video::s_viewportWidth))
        {
            ctx.r3 = r3;
            ctx.r4 = ctx.r1;
            ctx.r5 = r5;
            original(ctx, base);

            if (isRepeatLeft)
            {
                for (size_t i = 0; i < r5.u32; i++)
                    getVertex(i)->X = getVertex(i)->X - width;
            }
            else if (isRepeatRight)
            {
                for (size_t i = 0; i < r5.u32; i++)
                    getVertex(i)->X = getVertex(i)->X + width;
            }

            x = getVertex(vertexIndex)->X;

            auto isFlipHorz = (modifier.Flags & REPEAT_FLIP_HORIZONTAL) != 0;

            if (isFlipHorz)
            {
                getVertex(0)->X = getVertex(0)->X + width;
                getVertex(1)->X = getVertex(1)->X + width;
                getVertex(2)->X = getVertex(2)->X - width;
                getVertex(3)->X = getVertex(3)->X - width;
            }

            if ((modifier.Flags & REPEAT_UV_MODIFIER) != 0)
            {
                getVertex(0)->U = getVertex(0)->U + modifier.UVs.U0;
                getVertex(0)->V = getVertex(0)->V + modifier.UVs.V0;
                getVertex(1)->U = getVertex(1)->U + modifier.UVs.U1;
                getVertex(1)->V = getVertex(1)->V + modifier.UVs.V1;
                getVertex(2)->U = getVertex(2)->U + modifier.UVs.U2;
                getVertex(2)->V = getVertex(2)->V + modifier.UVs.V2;
                getVertex(3)->U = getVertex(3)->U + modifier.UVs.U3;
                getVertex(3)->V = getVertex(3)->V + modifier.UVs.V3;
            }

            if ((modifier.Flags & REPEAT_COLOUR_MODIFIER) != 0)
            {
                getVertex(0)->Colour = modifier.Colours.C0;
                getVertex(1)->Colour = modifier.Colours.C1;
                getVertex(2)->Colour = modifier.Colours.C2;
                getVertex(3)->Colour = modifier.Colours.C3;
            }

            if ((modifier.Flags & REPEAT_EXTEND) != 0)
            {
                for (size_t i = 0; i < r5.u32; i++)
                {
                    if (isRepeatLeft && (i == (isFlipHorz ? 2 : 0) || i == (isFlipHorz ? 3 : 1)))
                    {
                        getVertex(i)->X = std::min(getVertex(i)->X.get(), 0.0f);
                    }
                    else if (isRepeatRight && (i == (isFlipHorz ? 0 : 2) || i == (isFlipHorz ? 1 : 3)))
                    {
                        getVertex(i)->X = std::max(getVertex(i)->X.get(), float(Video::s_viewportWidth));
                    }
                }
            }
        }

        ctx.r1.u32 += size;
    }
    else
    {
        ctx.r4.u32 = ctx.r1.u32;
        original(ctx, base);
        ctx.r1.u32 += size;
    }
}

// Sonicteam::CPlatformMarathon::Draw
PPC_FUNC_IMPL(__imp__sub_826315C8);
PPC_FUNC(sub_826315C8)
{
    // r3 = Sonicteam::CPlatformMarathon*
    // r4 = pointer to vertices
    // r5 = vertex count

    Draw(ctx, base, __imp__sub_826315C8, 0x14);
}

// Sonicteam::CPlatformMarathon::DrawNoTex
PPC_FUNC_IMPL(__imp__sub_82631718);
PPC_FUNC(sub_82631718)
{
    // r3 = Sonicteam::CPlatformMarathon*
    // r4 = pointer to vertices
    // r5 = vertex count

    Draw(ctx, base, __imp__sub_82631718, 0x0C);
}

// Sonicteam::HUDRaderMap::Update
PPC_FUNC_IMPL(__imp__sub_824F1538);
PPC_FUNC(sub_824F1538)
{
    auto pHUDRaderMap = (Sonicteam::HUDRaderMap*)(base + ctx.r3.u32);

    __imp__sub_824F1538(ctx, base);

    auto radermapScale = 256 * g_aspectRatioScale * g_aspectRatioGameplayScale;

    pHUDRaderMap->m_pMaskTexture->m_Width = radermapScale;
    pHUDRaderMap->m_pMaskTexture->m_Height = radermapScale;
    pHUDRaderMap->m_X = g_radarMapX - g_radarMapCoverWidth / 2;
    pHUDRaderMap->m_Y = g_radarMapY - g_radarMapCoverHeight / 2;
}

void ReplaceTextVariables(Sonicteam::TextEntity* pTextEntity, xxHashMap<TextFontPictureParams> pftParams)
{
    if (!pTextEntity || !pTextEntity->m_ImageVertexCount)
        return;

    auto variables = MapTextVariables(pTextEntity->m_pVariables);
    auto variablesIndex = 0;
    auto pictureIndex = 0;

    for (int i = 0; i < pTextEntity->m_TextLength * 2; i++)
    {
        auto c = ByteSwap(pTextEntity->m_pText[i]);

        if (c != L'$')
            continue;

        // Some strings may have more placeholder
        // characters than variables in the map.
        if (variablesIndex >= variables.size())
            break;

        auto& variable = variables[variablesIndex];

        if (variable.first == "picture")
        {
            auto w = g_pTextFontPicture->m_TextureWidth;
            auto h = g_pTextFontPicture->m_TextureHeight;

            auto baseParams = FindFontPictureParams(g_pftParamsXenon, variable.second);
            auto newParams = FindFontPictureParams(pftParams, variable.second);

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

static float g_textScaleY;
static float g_textHeight;

// Sonicteam::TextEntity::Draw
PPC_FUNC_IMPL(__imp__sub_8262D868);
PPC_FUNC(sub_8262D868)
{
    auto pTextEntity = (Sonicteam::TextEntity*)(base + ctx.r3.u32);

    // LOGFN_UTILITY("TextEntity: 0x{:08X}", (uint64_t)pTextEntity);

    constexpr auto baseWidth = 1280.0f;
    constexpr auto baseHeight = 720.0f;

    auto scaleFactor = 1.0f;
    auto scaleY = 1.0f;
    auto offsetY = (baseHeight - (baseHeight * scaleY)) / 2.0f;

    if (g_aspectRatio < WIDE_ASPECT_RATIO)
    {
        scaleFactor = 1.125f;
        scaleY = g_aspectRatio / WIDE_ASPECT_RATIO;

        pTextEntity->m_Y = pTextEntity->m_Y * scaleY + offsetY;
        pTextEntity->m_ScaleY = scaleY * scaleFactor;

        g_textScaleY = pTextEntity->m_ScaleY;
    }

    auto scaleX = 1.0f * (WIDE_ASPECT_RATIO / g_aspectRatio) * scaleY;
    auto scaledWidth = baseWidth * scaleX;
    auto offsetX = (baseWidth - scaledWidth) / 2.0f;

    pTextEntity->m_X = pTextEntity->m_X * scaleX + offsetX;
    pTextEntity->m_ScaleX = scaleX * scaleFactor;

    __imp__sub_8262D868(ctx, base);

    auto isPlayStation = Config::ControllerIcons == EControllerIcons::PlayStation;

    if (Config::ControllerIcons == EControllerIcons::Auto)
        isPlayStation = hid::g_inputDeviceController == hid::EInputDevice::PlayStation;

    auto& pftParams = isPlayStation
        ? g_pftParamsPS3
        : g_pftParamsXenon;

    // TODO (Hyper): why does this not apply to the B button in Audio Room?
    ReplaceTextVariables(pTextEntity, pftParams);
}

// Sonicteam::HUDButtonWindow::Draw
PPC_FUNC_IMPL(__imp__sub_824D4628);
PPC_FUNC(sub_824D4628)
{
    auto pHUDButtonWindow = (Sonicteam::HUDButtonWindow*)g_memory.Translate(ctx.r3.u32);

    auto width = Video::s_viewportWidth;
    auto height = Video::s_viewportHeight;
    auto baseWidth = height * WIDE_ASPECT_RATIO;
    auto baseHeight = width / WIDE_ASPECT_RATIO;

    auto horizontalBar = 0.0f;
    auto verticalBar = 0.0f;

    if (width < baseWidth)
        horizontalBar = (baseWidth - width) / 2.0f;

    if (height < baseHeight)
        verticalBar = (baseHeight - height) / 2.0f;

    // LOGFN_UTILITY("vert: {}", verticalBar);
    // LOGFN_UTILITY("horz: {}", horizontalBar);

    // pHUDButtonWindow->m_pHudTextParts->m_OffsetX = -55.0f;

    if (horizontalBar > 0)
        pHUDButtonWindow->m_pHudTextParts->m_OffsetY = horizontalBar;

    __imp__sub_824D4628(ctx, base);
}

// Sonicteam::HudTextParts::HudTextParts
PPC_FUNC_IMPL(__imp__sub_824D04C8);
PPC_FUNC(sub_824D04C8)
{
    auto pTextBookName = (const char*)g_memory.Translate(ctx.r7.u32);
    auto pTextCardName = (const char*)g_memory.Translate(ctx.r8.u32);

    __imp__sub_824D04C8(ctx, base);

    auto pHudTextParts = (Sonicteam::HudTextParts*)g_memory.Translate(ctx.r3.u32);
}

// Sonicteam::TextFontPicture::LoadResource
PPC_FUNC_IMPL(__imp__sub_8263CC40);
PPC_FUNC(sub_8263CC40)
{
    g_pTextFontPicture = (Sonicteam::TextFontPicture*)(base + ctx.r3.u32);

    __imp__sub_8263CC40(ctx, base);
}
