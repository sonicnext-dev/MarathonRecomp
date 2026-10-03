#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#ifdef __spirv__

#define s0_Texture2DDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 0)
#define s1_Texture2DDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 4)
#define s2_Texture2DDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 8)
#define s0_SamplerDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 192)
#define s1_SamplerDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 196)
#define s2_SamplerDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 200)

#else

cbuffer SharedConstants : register(b2, space4)
{
    uint s0_Texture2DDescriptorIndex : packoffset(c0.x);
    uint s1_Texture2DDescriptorIndex : packoffset(c0.y);
    uint s2_Texture2DDescriptorIndex : packoffset(c0.z);
    uint s0_SamplerDescriptorIndex : packoffset(c12.x);
    uint s1_SamplerDescriptorIndex : packoffset(c12.y);
    uint s2_SamplerDescriptorIndex : packoffset(c12.z);
    DEFINE_SHARED_CONSTANTS();
};

#endif

float4 shaderMain(
    in float4 oPos : SV_Position,
    in float4 oTexCoord0 : TEXCOORD0) : SV_Target
{
    Texture2D<float4> textureY = g_Texture2DDescriptorHeap[s0_Texture2DDescriptorIndex];
    Texture2D<float4> textureU = g_Texture2DDescriptorHeap[s1_Texture2DDescriptorIndex];
    Texture2D<float4> textureV = g_Texture2DDescriptorHeap[s2_Texture2DDescriptorIndex];
    SamplerState samplerStateY = g_SamplerDescriptorHeap[s0_SamplerDescriptorIndex];
    SamplerState samplerStateU = g_SamplerDescriptorHeap[s1_SamplerDescriptorIndex];
    SamplerState samplerStateV = g_SamplerDescriptorHeap[s2_SamplerDescriptorIndex];

    float y = (textureY.SampleLevel(samplerStateY, oTexCoord0.xy, 0).x - 0.0625) * 1.164;
    float u = textureU.SampleLevel(samplerStateU, oTexCoord0.xy, 0).x - 0.5;
    float v = textureV.SampleLevel(samplerStateV, oTexCoord0.xy, 0).x - 0.5;

    return float4(y + 1.596 * v, y - 0.392 * u - 0.813 * v, y + 2.017 * u, 1.0);
}
