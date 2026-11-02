#include "gpu/video.h"

#include <api/Marathon.h>
#include <kernel/memory.h>
#include <os/logger.h>
#include <patches/aspect_ratio_patches.h>
#include <user/config.h>
#include <gpu/render_resolution.h>

const char* g_pBlockName{};

extern const char* g_surfaceCreationName;

void SetMSAALevel(PPCRegister& val)
{
    val.u32 = 0;
}

void ScaleDepthStencilMidAsmHook(PPCRegister& width, PPCRegister& height)
{
    const auto internalSize = RenderResolution::GetInternalSize();
    width.u32 = internalSize.Width;
    height.u32 = internalSize.Height;
}

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

static GuestShader* g_movieVertexShader;
static GuestShader* g_moviePixelShaderHD;
static GuestShader* g_moviePixelShaderSD;

static int ScreenShaderInit(be<uint32_t>* a1)
{
    if (g_movieVertexShader == nullptr)
    {
        g_movieVertexShader = g_userHeap.AllocPhysical<GuestShader>(ResourceType::VertexShader);
    }

    if (g_moviePixelShaderHD == nullptr)
    {
        g_moviePixelShaderHD = g_userHeap.AllocPhysical<GuestShader>(ResourceType::PixelShader);
    }

    if (g_moviePixelShaderSD == nullptr)
    {
        g_moviePixelShaderSD = g_userHeap.AllocPhysical<GuestShader>(ResourceType::PixelShader);
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
