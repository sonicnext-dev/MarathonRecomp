#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#define s0_TextureDescriptorIndex (*(reinterpret_cast<device int*>(g_PushConstants.SharedConstants + 0)))
#define s0_SamplerDescriptorIndex (*(reinterpret_cast<device uint*>(g_PushConstants.SharedConstants + 192)))

struct Interpolators
{
    float4 oPos [[position]];
    float4 oTexCoord0 [[user(TEXCOORD0)]];
};

[[fragment]]
float4 shaderMain(Interpolators input [[stage_in]],
                  constant Texture2DDescriptorHeap* g_Texture2DDescriptorHeap [[buffer(0)]],
                  constant SamplerDescriptorHeap* g_SamplerDescriptorHeap [[buffer(3)]],
                  constant PushConstants& g_PushConstants [[buffer(8)]])
{
    texture2d<float> texture = g_Texture2DDescriptorHeap[s0_TextureDescriptorIndex].tex;
    sampler samp = g_SamplerDescriptorHeap[s0_SamplerDescriptorIndex].samp;

    float2 dimensions = float2(getTexture2DDimensions(texture));
    float2 pixelCoord = input.oTexCoord0.xy * dimensions;
    float2 texelsPerPixel = float2(dfdx(pixelCoord.x), dfdy(pixelCoord.y));

    pixelCoord += (texelsPerPixel - 1.0) * 0.5;

    return texture.sample(samp, pixelCoord / dimensions);
}
