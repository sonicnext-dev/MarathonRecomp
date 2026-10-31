#pragma once

#include "video.h"

#if !defined(MARATHON_RECOMP_D3D12) && !defined(MARATHON_RECOMP_METAL)
inline constexpr Backend g_backend = Backend::VULKAN;
#else
extern Backend g_backend;
#endif

#if defined(MARATHON_RECOMP_D3D12)

#define CREATE_SHADER(NAME) \
    Video::GetRenderDevice()->createShader( \
        (g_backend == Backend::VULKAN) ? g_##NAME##_spirv : g_##NAME##_dxil, \
        (g_backend == Backend::VULKAN) ? sizeof(g_##NAME##_spirv) : sizeof(g_##NAME##_dxil), \
        "shaderMain", \
        (g_backend == Backend::VULKAN) ? RenderShaderFormat::SPIRV : RenderShaderFormat::DXIL)

#elif defined(MARATHON_RECOMP_METAL)

#define CREATE_SHADER(NAME) \
    Video::GetRenderDevice()->createShader( \
        (g_backend == Backend::VULKAN) ? g_##NAME##_spirv : g_##NAME##_air, \
        (g_backend == Backend::VULKAN) ? sizeof(g_##NAME##_spirv) : sizeof(g_##NAME##_air), \
        "shaderMain", \
        (g_backend == Backend::VULKAN) ? RenderShaderFormat::SPIRV : RenderShaderFormat::METAL)

#else

#define CREATE_SHADER(NAME) \
    Video::GetRenderDevice()->createShader(g_##NAME##_spirv, sizeof(g_##NAME##_spirv), "shaderMain", RenderShaderFormat::SPIRV)

#endif

extern std::thread::id g_mainThreadId;
extern std::unique_ptr<RenderPipelineLayout> g_pipelineLayout;

void DiscardTexture(GuestBaseTexture* texture, RenderTextureLayout layout);
