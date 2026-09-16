#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#define g_Gamma (*(reinterpret_cast<device float*>(g_PushConstants.SharedConstants + 0)))
#define g_TextureDescriptorIndex (*(reinterpret_cast<device uint*>(g_PushConstants.SharedConstants + 4)))

#define g_ViewportOffset (*(reinterpret_cast<device int2*>(g_PushConstants.SharedConstants + 8)))
#define g_ViewportSize (*(reinterpret_cast<device int2*>(g_PushConstants.SharedConstants + 16)))
#define g_SourceSize (*(reinterpret_cast<device int2*>(g_PushConstants.SharedConstants + 24)))

[[fragment]]
float4 shaderMain(float4 position [[position]],
                  constant Texture2DDescriptorHeap* g_Texture2DDescriptorHeap [[buffer(0)]],
                  constant PushConstants& g_PushConstants [[buffer(8)]])
{
    texture2d<float> texture = g_Texture2DDescriptorHeap[g_TextureDescriptorIndex].tex;

    int2 viewportSize = g_ViewportSize;
    int2 sourceSize = g_SourceSize;

    int2 movedPosition = int2(position.xy) - g_ViewportOffset;
    bool boxed = any(movedPosition < 0) || any(movedPosition >= viewportSize);

    if (boxed)
        return float4(0.0);

    // The guest renders at a fixed resolution, so scale it into the destination
    // rectangle. Bilinear filtering is done manually to avoid needing a sampler.
    float2 sourcePosition = (float2(movedPosition) + 0.5) *
        (float2(sourceSize) / float2(viewportSize)) - 0.5;

    float2 sourceFloor = floor(sourcePosition);
    float2 fraction = sourcePosition - sourceFloor;

    int2 maxCoord = sourceSize - 1;
    int2 coord0 = clamp(int2(sourceFloor), int2(0), maxCoord);
    int2 coord1 = clamp(coord0 + 1, int2(0), maxCoord);

    float4 s00 = texture.read(uint2(coord0.x, coord0.y), 0);
    float4 s10 = texture.read(uint2(coord1.x, coord0.y), 0);
    float4 s01 = texture.read(uint2(coord0.x, coord1.y), 0);
    float4 s11 = texture.read(uint2(coord1.x, coord1.y), 0);

    float4 color = mix(mix(s00, s10, fraction.x), mix(s01, s11, fraction.x), fraction.y);

    color.rgb = pow(color.rgb, g_Gamma);

    return color;
}
