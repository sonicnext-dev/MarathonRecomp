#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#ifdef __spirv__

#define g_Gamma vk::RawBufferLoad<float>(g_PushConstants.SharedConstants + 0)
#define g_TextureDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 4)

#define g_ViewportOffset vk::RawBufferLoad<int2>(g_PushConstants.SharedConstants + 8)
#define g_ViewportSize vk::RawBufferLoad<int2>(g_PushConstants.SharedConstants + 16)
#define g_SourceSize vk::RawBufferLoad<int2>(g_PushConstants.SharedConstants + 24)

#else

cbuffer SharedConstants : register(b2, space4)
{
    float g_Gamma;
    uint g_TextureDescriptorIndex;
    int2 g_ViewportOffset;
    int2 g_ViewportSize;
    int2 g_SourceSize;
};

#endif

float4 shaderMain(in float4 position : SV_Position) : SV_Target
{
    Texture2D<float4> texture = g_Texture2DDescriptorHeap[g_TextureDescriptorIndex];

    int2 viewportSize = g_ViewportSize;
    int2 sourceSize = g_SourceSize;

    int2 movedPosition = int2(position.xy) - g_ViewportOffset;
    bool boxed = any(movedPosition < 0) || any(movedPosition >= viewportSize);

    if (boxed)
        return 0.0;

    // The guest renders at a fixed resolution, so scale it into the destination
    // rectangle. Bilinear filtering is done manually to avoid needing a sampler.
    float2 sourcePosition = (float2(movedPosition) + 0.5) *
        (float2(sourceSize) / float2(viewportSize)) - 0.5;

    float2 sourceFloor = floor(sourcePosition);
    float2 fraction = sourcePosition - sourceFloor;

    int2 maxCoord = sourceSize - 1;
    int2 coord0 = clamp(int2(sourceFloor), int2(0, 0), maxCoord);
    int2 coord1 = clamp(coord0 + 1, int2(0, 0), maxCoord);

    float4 s00 = texture.Load(int3(coord0.x, coord0.y, 0));
    float4 s10 = texture.Load(int3(coord1.x, coord0.y, 0));
    float4 s01 = texture.Load(int3(coord0.x, coord1.y, 0));
    float4 s11 = texture.Load(int3(coord1.x, coord1.y, 0));

    float4 color = lerp(lerp(s00, s10, fraction.x), lerp(s01, s11, fraction.x), fraction.y);

    color.rgb = pow(color.rgb, g_Gamma);

    return color;
}
