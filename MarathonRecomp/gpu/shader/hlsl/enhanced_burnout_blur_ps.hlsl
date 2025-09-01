#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#ifdef __spirv__

#define g_SampleCount vk::RawBufferLoad<int>(g_PushConstants.PixelShaderConstants + 0)

#define s0_TextureDescriptorIndex vk::RawBufferLoad<int>(g_PushConstants.SharedConstants + 0)
#define s0_SamplerDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 192)

#else

cbuffer PixelShaderConstants : register(b1, space1)
{
    int g_SampleCount;
};

cbuffer SharedConstants : register(b2, space4)
{
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
    Texture2D<float4> texture = g_Texture2DDescriptorHeap[s0_TextureDescriptorIndex];
    SamplerState samplerState = g_SamplerDescriptorHeap[s0_SamplerDescriptorIndex];

    if (g_SampleCount == 1)
        return texture.SampleLevel(samplerState, oTexCoord0, 0);

    float velocityMag = sqrt(dot(oVelocity, oVelocity));
    float blurAmount = saturate(velocityMag - 0.25);

    if (blurAmount == 0.0)
        return texture.SampleLevel(samplerState, oTexCoord0, 0);

    blurAmount = min(blurAmount * blurAmount, 0.25);
    float4 result = float4(0.0, 0.0, 0.0, 0.0);

    UNROLL
    for (int i = 0; i < g_SampleCount; i++)
    {
        float t = ((float)i / (float)(g_SampleCount - 1)) * 2.0 - 1.0;

        float2 offset = blurAmount * oVelScale * t * g_SampleCount;
        float2 samplePos = oTexCoord0 + offset;

        result += texture.SampleLevel(samplerState, samplePos, 0);
    }

    return result * (1.0 / (float)g_SampleCount);
}