#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#ifdef __spirv__

#define g_SampleCount vk::RawBufferLoad<int>(g_PushConstants.SharedConstants + 4)

#define s0_TextureDescriptorIndex vk::RawBufferLoad<int>(g_PushConstants.SharedConstants + 0)
#define s0_SamplerDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 192)

#else

cbuffer SharedConstants : register(b2, space4)
{
    int g_SampleCount : packoffset(c0.y);
    uint s0_Texture2DDescriptorIndex : packoffset(c0.x);
    uint s0_SamplerDescriptorIndex : packoffset(c12.x);
    DEFINE_SHARED_CONSTANTS();
};

#endif

float4 shaderMain(
    in float4 oPos : SV_Position,
    in float2 oTexCoord0 : TEXCOORD0,
    in float2 oVelocity : TEXCOORD1,
    in float2 oVelScale : TEXCOORD2) : SV_Target
{
    if (g_SampleCount == 1)
        return texture.SampleLevel(SAMPLER, oTexCoord0, 0);

    float velocityMag = sqrt(dot(oVelocity, oVelocity));
    float blurAmount = saturate(velocityMag - 0.25);

    if (blurAmount == 0.0)
        return texture.SampleLevel(SAMPLER, oTexCoord0, 0);

    blurAmount = min(blurAmount * blurAmount, 0.25);
    float result = float4(0.0);

    Texture2D<float> texture = g_Texture2DDescriptorHeap[s0_TextureDescriptorIndex];

    UNROLL
    for (int i = 0; i < g_SampleCount; i++)
    {
        float t = ((float)i / (float)(g_SampleCount - 1)) * 2.0 - 1.0;

        float2 offset = blurAmount * oVelScale * t * g_SampleCount;
        float2 samplePos = oTexCoord0 + offset;

        result += texture.SampleLevel(s0_SamplerDescriptorIndex, samplePos, 0);
    }

    return result * (1.0 / (float)g_SampleCount);
}