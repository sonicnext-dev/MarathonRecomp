#include <api/Marathon.h>
#include <user/config.h>

void SetMSAALevel(PPCRegister& val)
{
    val.u32 = 0;
}

std::string g_blockName;

void BeginBlock(PPCRegister& name)
{
    auto pName = xpointer(reinterpret_cast<char*>(name.u32));
    g_blockName = std::string(pName.get());
}

// Blocks can be nested, so this is actually
// incorrect, but we only care about one that isn't
void EndBlock()
{
    g_blockName.clear();
}

float ReflectionScaleFactor(EReflectionResolution ref) {
    switch (ref) {
        case EReflectionResolution::Eighth:
            return 0.5f;
        case EReflectionResolution::Quarter:
            return 1.0f;
        case EReflectionResolution::Half:
            return 2.0f;
        case EReflectionResolution::Full:
            return 4.0f;
        default:
            return 1.0f;
    }
}

// CreateTexture
PPC_FUNC_IMPL(__imp__sub_82619D00);
PPC_FUNC(sub_82619D00)
{
    auto pName = (stdx::string*)g_memory.Translate(ctx.r4.u32);

    if (*pName == "reflection0")
    {
        ctx.r5.u32 = static_cast<int>(static_cast<float>(ctx.r5.u32) *
            ReflectionScaleFactor(Config::ReflectionResolution));
        ctx.r6.u32 = static_cast<int>(static_cast<float>(ctx.r6.u32) *
            ReflectionScaleFactor(Config::ReflectionResolution));
    }

    // RenderMefiress
    if (*pName == "user0")
    {
        ctx.r5.u32 = (int32_t)Config::ShadowResolution.Value;
        ctx.r6.u32 = (int32_t)Config::ShadowResolution.Value;
    }

    __imp__sub_82619D00(ctx, base);
}

// CreateDepthStencilTexture
PPC_FUNC_IMPL(__imp__sub_82619B88);
PPC_FUNC(sub_82619B88)
{
    auto pName = (stdx::string*)g_memory.Translate(ctx.r4.u32);

    if (*pName == "depthstencil_1_4")
    {
        ctx.r5.u32 = static_cast<int>(static_cast<float>(ctx.r5.u32) *
            ReflectionScaleFactor(Config::ReflectionResolution));
        ctx.r6.u32 = static_cast<int>(static_cast<float>(ctx.r6.u32) *
            ReflectionScaleFactor(Config::ReflectionResolution));

        // Bad hack to stop EDRAM cache from messing up
        if (Config::ReflectionResolution == EReflectionResolution::Full)
            ctx.r5.u32++;
    }

    // RenderMefiress
    if (*pName == "depthstencil_256" && g_blockName == "user0")
    {
        ctx.r5.u32 = (int32_t)Config::ShadowResolution.Value;
        ctx.r6.u32 = (int32_t)Config::ShadowResolution.Value;
    }

    __imp__sub_82619B88(ctx, base);
}

float ShadowScaleFactor(EShadowResolution ref) {
    switch (ref) {
        case EShadowResolution::x512:
            return 0.5f;
        case EShadowResolution::x1024:
            return 1.0f;
        case EShadowResolution::x2048:
            return 2.0f;
        case EShadowResolution::x4096:
            return 4.0f;
        case EShadowResolution::x8192:
            return 8.0f;
        default:
            return 1.0f;
    }
}

// CreateArrayTexture
PPC_FUNC_IMPL(__imp__sub_82619FF0);
PPC_FUNC(sub_82619FF0)
{
    auto pName = (stdx::string*)g_memory.Translate(ctx.r4.u32);

    if (*pName == "csm")
    {
        ctx.r5.u32 = static_cast<int>(static_cast<float>(ctx.r5.u32) *
            ShadowScaleFactor(Config::ShadowResolution));
        ctx.r6.u32 = static_cast<int>(static_cast<float>(ctx.r6.u32) *
            ShadowScaleFactor(Config::ShadowResolution));
    }

    __imp__sub_82619FF0(ctx, base);
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
