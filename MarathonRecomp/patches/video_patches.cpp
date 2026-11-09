#include <api/Marathon.h>
#include <gpu/render_resolution.h>
#include <gpu/video.h>
#include <kernel/memory.h>
#include <os/logger.h>
#include <user/config.h>

const char* g_pBlockName{};

extern const char* g_surfaceCreationName;

void BeginBlockGetName(PPCRegister& r3)
{
    g_pBlockName = (const char*)g_memory.Translate(r3.u32);

#if _DEBUG
    if (g_pBlockName)
        LOGFN_UTILITY("Block Begin: {}", g_pBlockName);
#endif
}

// EndBlock
PPC_FUNC_IMPL(__imp__sub_826078D8);
PPC_FUNC(sub_826078D8)
{
#if _DEBUG
    if (g_pBlockName)
        LOGFN_UTILITY("Block End: {}", g_pBlockName);
#endif

    g_pBlockName = nullptr;

    __imp__sub_826078D8(ctx, base);
}

static void ApplyBufferSize(PPCContext& ctx, const char* pName)
{
    if (auto size = RenderResolution::FindBufferSize(pName, g_pBlockName))
    {
        ctx.r5.u32 = size->Width;
        ctx.r6.u32 = size->Height;
    }
}

// CreateTexture
PPC_FUNC_IMPL(__imp__sub_82619D00);
PPC_FUNC(sub_82619D00)
{
    auto pName = (stdx::string*)g_memory.Translate(ctx.r4.u32);
    ApplyBufferSize(ctx, pName->c_str());

#if _DEBUG
    auto width = ctx.r5.u32;
    auto height = ctx.r6.u32;
#endif

    g_surfaceCreationName = pName->c_str();
    __imp__sub_82619D00(ctx, base);
    g_surfaceCreationName = nullptr;

#if _DEBUG
    LOGFN_UTILITY("Created texture: {} ({}x{})", pName->c_str(), width, height);
#endif
}

// CreateDepthStencilSurface
PPC_FUNC_IMPL(__imp__sub_82619B88);
PPC_FUNC(sub_82619B88)
{
    auto pName = (stdx::string*)g_memory.Translate(ctx.r4.u32);
    ApplyBufferSize(ctx, pName->c_str());

#if _DEBUG
    auto width = ctx.r5.u32;
    auto height = ctx.r6.u32;
#endif

    g_surfaceCreationName = pName->c_str();
    __imp__sub_82619B88(ctx, base);
    g_surfaceCreationName = nullptr;

#if _DEBUG
    if (g_pBlockName)
    {
        LOGFN_UTILITY("Created texture for {}: {} ({}x{})", g_pBlockName, pName->c_str(), width, height);
    }
    else
    {
        LOGFN_UTILITY("Created texture: {} ({}x{})", pName->c_str(), width, height);
    }
#endif
}

// CreateArrayTexture
PPC_FUNC_IMPL(__imp__sub_82619FF0);
PPC_FUNC(sub_82619FF0)
{
    auto pName = (stdx::string*)g_memory.Translate(ctx.r4.u32);
    ApplyBufferSize(ctx, pName->c_str());

    g_surfaceCreationName = pName->c_str();
    __imp__sub_82619FF0(ctx, base);
    g_surfaceCreationName = nullptr;
}

// Sonicteam::MyReflectionArea::Update
PPC_FUNC_IMPL(__imp__sub_822740A8);
PPC_FUNC(sub_822740A8)
{
    const auto pMyReflectionArea = reinterpret_cast<Sonicteam::MyReflectionArea*>(base + ctx.r3.u32);

    // Force planar reflections to draw all objects and terrain.
    pMyReflectionArea->m_ClumpCutoffCoverage = 0.0f;
    pMyReflectionArea->m_SectorCutoffCoverage = 0.0f;

    __imp__sub_822740A8(ctx, base);

    if (pMyReflectionArea->m_IsInsideArea)
        return;

    const auto pGame = static_cast<Sonicteam::GameImp*>(pMyReflectionArea->m_spGame.get());
    const auto pConvexShape = static_cast<Sonicteam::SoX::Physics::Havok::ConvexShapeHavok*>(pMyReflectionArea->m_pConvexShape.get());

    for (const auto& rActorId : pGame->m_PlayerActorIDs)
    {
        const auto pPlayer = pGame->m_spActorManager->Find<Sonicteam::Player::Object>(rActorId);

        if (!pPlayer)
            continue;

        const auto pCameraman = static_cast<Sonicteam::Camera::Cameraman*>(pPlayer->m_pCameraman.get());

        if (!pCameraman)
            continue;

        // Fix reflection areas not updating with camera inside.
        pMyReflectionArea->m_IsInsideArea = pConvexShape->m_pConvexVerticesShape->hasPoint(pCameraman->m_Transform.m_Position);
    }
}

std::string g_renderWorldFBO;

void GetRenderWorldFBO(PPCRegister& name)
{
    auto pName = xpointer(reinterpret_cast<char*>(name.u32));
    g_renderWorldFBO = std::string(pName.get());
}

void FurtherObjectShadows(PPCRegister& scope)
{
    if (g_renderWorldFBO != "shadowmap")
        return;

    scope.u32 = 1;
}

bool DisableRadialBlur()
{
    return Config::RadialBlur == ERadialBlur::Off;
}

bool DisableKingdomValleyMist()
{
    return Config::DisableKingdomValleyMist;
}

static thread_local bool g_isLoadingEnvMap = false;

void SceneEnvMapLoadTexture()
{
    if (Config::FixEnvironmentMaps)
        g_isLoadingEnvMap = true;
}

void ResourceMgrKeyOverride(PPCRegister& r1, PPCRegister& r28)
{
    if (!g_isLoadingEnvMap)
        return;

    g_isLoadingEnvMap = false;

    // std::string::assign(const std::string&, uint, uint)
    GuestToHostFunction<void>(0x82161D10, r1.u32 + 0x60, r28.u32, 0, 0xFFFFFFFF);
}

void IndexBufferLengthMidAsmHook(PPCRegister& r3)
{
    r3.u64 *= 2;
}

void SetShadowResolutionMidAsmHook(PPCRegister& r11)
{
    auto res = (int32_t)Config::ShadowResolution.Value;

    if (res > 0)
        r11.u64 = res;
}

void SetMsaaLevel(PPCRegister& r1)
{
    const auto pLuaSystem = reinterpret_cast<xpointer<Sonicteam::LuaSystem>*>(g_memory.Translate(r1.u32 + 0x54))->get();

    // Force Lua MSAA type to 2x to override stages that disable it.
    pLuaSystem->SetString("MSAAType", "2x");
}

void SetMsaaLevelForBuffers(PPCRegister& r31, PPCRegister& r4)
{
    const auto L = reinterpret_cast<lua50::lua_State*>(g_memory.Translate(r31.u32));

    // MSAA type is provided, abort.
    if (lua50::lua_isstring(L, 9))
        return;

    const auto pBufferName = lua50::lua_tostring(L, 2);
    const auto pDepthStencilName = lua50::lua_tostring(L, 3);
    const auto bufferType = Sonicteam::SoX::Graphics::GetBufferTypeByName(lua50::lua_tostring(L, 4));

    static const std::array<std::tuple<const char*, const char*, Sonicteam::SoX::Graphics::BufferType>, 2> buffers =
    {{
        // Force MSAA level for multiplayer.
        { "coframebuffer", "codepthstencil", Sonicteam::SoX::Graphics::BufferType_All },

        // Force MSAA level for radar.
        { "radermap", "depthstencil_256", Sonicteam::SoX::Graphics::BufferType_ColorDepth }
    }};

    for (const auto& [rBufferName, rDepthStencilName, rBufferType] : buffers)
    {
        if (strstr(pBufferName, rBufferName) && strcmp(pDepthStencilName, rDepthStencilName) == 0 && bufferType == rBufferType)
            r4.u32 = Config::AntiAliasing != EAntiAliasing::Off;
    }
}

void FixSurfaceSize(PPCRegister& r3, PPCRegister& r4)
{
    const auto internalSize = RenderResolution::GetInternalSize();

    r3.u32 = internalSize.Width;
    r4.u32 = internalSize.Height;
}

void FixFrameBufferTileRects(PPCRegister& r26, PPCRegister& r1)
{
    const auto pFrameBufferObjectXenon = reinterpret_cast<Sonicteam::SoX::Graphics::Xenon::FrameBufferObjectXenon*>(g_memory.Translate(r26.u32));
    const auto pSurfaceDesc = reinterpret_cast<D3DXBSURFACE_DESC*>(g_memory.Translate(r1.u32 + 0x80));

    // Tiling is disabled, always use a single rect.
    D3DXBRECT rect{ 0, 0, pSurfaceDesc->Width.get(), pSurfaceDesc->Height.get() };
    pFrameBufferObjectXenon->m_TileRects.push_back(rect);
}

bool ClearMsaaFrameBuffer(PPCRegister& r31)
{
    const auto pFrameBufferObjectXenon = reinterpret_cast<Sonicteam::SoX::Graphics::Xenon::FrameBufferObjectXenon*>(g_memory.Translate(r31.u32));

    // Ensure an MSAA framebuffer is cleared before re-use.
    return pFrameBufferObjectXenon->m_MSAALevel != 0;
}

void ResolveMsaaFrameBuffers(PPCRegister& r31, PPCRegister& r8)
{
    const auto pFrameBufferObjectXenon = reinterpret_cast<Sonicteam::SoX::Graphics::Xenon::FrameBufferObjectXenon*>(g_memory.Translate(r31.u32));

    // Set bit mask to always resolve color targets for MSAA framebuffers.
    if (pFrameBufferObjectXenon->m_MSAALevel)
        r8.u32 = 0xF;
}

bool CopyResolvedMsaaFrameBuffer(PPCRegister& r27)
{
    const auto pFrameBufferObjectXenon = *reinterpret_cast<xpointer<Sonicteam::SoX::Graphics::Xenon::FrameBufferObjectXenon>*>(g_memory.Translate(r27.u32));

    // Ensure an MSAA framebuffer's resolved output is copied.
    return pFrameBufferObjectXenon->m_MSAALevel != 0;
}

static GuestShader* g_movieVertexShader;
static GuestShader* g_moviePixelShaderHD;
static GuestShader* g_moviePixelShaderSD;

static int ScreenShaderInit(be<uint32_t>* a1)
{
    if (g_movieVertexShader == nullptr)
    {
        g_movieVertexShader = g_userHeap.AllocPhysical<GuestShader>();
        g_movieVertexShader->type = ResourceType::VertexShader;
    }

    if (g_moviePixelShaderHD == nullptr)
    {
        g_moviePixelShaderHD = g_userHeap.AllocPhysical<GuestShader>();
        g_moviePixelShaderHD->type = ResourceType::PixelShader;
    }

    if (g_moviePixelShaderSD == nullptr)
    {
        g_moviePixelShaderSD = g_userHeap.AllocPhysical<GuestShader>();
        g_moviePixelShaderSD->type = ResourceType::PixelShader;
    }

    g_moviePixelShaderHD->AddRef();
    g_moviePixelShaderSD->AddRef();
    g_movieVertexShader->AddRef();

    a1[0x12] = g_memory.MapVirtual(g_movieVertexShader);
    a1[0x51] = g_memory.MapVirtual(g_moviePixelShaderHD);
    a1[0x52] = g_memory.MapVirtual(g_moviePixelShaderSD);

    return 0;
}

GUEST_FUNCTION_HOOK(sub_82736178, ScreenShaderInit);
