#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#define s0_TextureDescriptorIndex (*(reinterpret_cast<device int*>(g_PushConstants.SharedConstants + 0)))

struct Interpolators
{
    float4 oPos [[position]];
    float4 oTexCoord0 [[user(TEXCOORD0)]];
};

[[fragment]]
float4 shaderMain(Interpolators input [[stage_in]],
                  constant Texture2DDescriptorHeap* g_Texture2DDescriptorHeap [[buffer(0)]],
                  constant PushConstants& g_PushConstants [[buffer(8)]])
{
    texture2d<float> texture = g_Texture2DDescriptorHeap[s0_TextureDescriptorIndex].tex;

    float2 pixelCoord = input.oTexCoord0.xy * float2(getTexture2DDimensions(texture));
    float2 step = float2(dfdx(pixelCoord.x), dfdy(pixelCoord.y)) / 4.0;

    float4 color = 0.0;

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
            color += texture.read(uint2(pixelCoord + (float2(x, y) - 1.5) * step));
    }

    return color / 16.0;
}
