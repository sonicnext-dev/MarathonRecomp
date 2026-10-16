#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#define s0_TextureDescriptorIndex (*(reinterpret_cast<device int*>(g_PushConstants.SharedConstants + 0)))
#define s1_TextureDescriptorIndex (*(reinterpret_cast<device int*>(g_PushConstants.SharedConstants + 4)))
#define s2_TextureDescriptorIndex (*(reinterpret_cast<device int*>(g_PushConstants.SharedConstants + 8)))
#define s0_SamplerDescriptorIndex (*(reinterpret_cast<device uint*>(g_PushConstants.SharedConstants + 192)))
#define s1_SamplerDescriptorIndex (*(reinterpret_cast<device uint*>(g_PushConstants.SharedConstants + 196)))
#define s2_SamplerDescriptorIndex (*(reinterpret_cast<device uint*>(g_PushConstants.SharedConstants + 200)))

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
    texture2d<float> textureY = g_Texture2DDescriptorHeap[s0_TextureDescriptorIndex].tex;
    texture2d<float> textureU = g_Texture2DDescriptorHeap[s1_TextureDescriptorIndex].tex;
    texture2d<float> textureV = g_Texture2DDescriptorHeap[s2_TextureDescriptorIndex].tex;
    sampler sampY = g_SamplerDescriptorHeap[s0_SamplerDescriptorIndex].samp;
    sampler sampU = g_SamplerDescriptorHeap[s1_SamplerDescriptorIndex].samp;
    sampler sampV = g_SamplerDescriptorHeap[s2_SamplerDescriptorIndex].samp;

    float y = (textureY.sample(sampY, input.oTexCoord0.xy, level(0)).x - 0.0625) * 1.164;
    float u = textureU.sample(sampU, input.oTexCoord0.xy, level(0)).x - 0.5;
    float v = textureV.sample(sampV, input.oTexCoord0.xy, level(0)).x - 0.5;

    return float4(y + 1.596 * v, y - 0.392 * u - 0.813 * v, y + 2.017 * u, 1.0);
}
