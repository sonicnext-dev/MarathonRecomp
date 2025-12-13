#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#define s0_TextureDescriptorIndex (*(reinterpret_cast<device int*>(g_PushConstants.SharedConstants + 0)))
#define s0_SamplerDescriptorIndex (*(reinterpret_cast<device uint*>(g_PushConstants.SharedConstants + 192)))

struct Interpolators
{
    float4 oPos [[position]];
    float2 oTexCoord0 [[user(TEXCOORD0)]];
    float2 oVelocity [[user(TEXCOORD1)]];
    float2 oVelScale [[user(TEXCOORD2)]];
};

[[fragment]]
float4 shaderMain(Interpolators input [[stage_in]],
                  constant Texture2DDescriptorHeap* g_Texture2DDescriptorHeap [[buffer(0)]],
                  constant SamplerDescriptorHeap* g_SamplerDescriptorHeap [[buffer(3)]],
                  constant PushConstants& g_PushConstants [[buffer(8)]])
{
    texture2d<float> texture = g_Texture2DDescriptorHeap[s0_TextureDescriptorIndex].tex;
    sampler samp = g_SamplerDescriptorHeap[s0_SamplerDescriptorIndex].samp;

    float velocityMag = sqrt(dot(input.oVelocity, input.oVelocity));
    float blurAmount = saturate(velocityMag - 0.25);

    if (blurAmount == 0.0)
        return texture.sample(samp, input.oTexCoord0, level(0));

    blurAmount = min(blurAmount * blurAmount, 0.25);

    float blurStrength = blurAmount * velocityMag;
    int g_SampleCount = clamp((int)(blurStrength * 256.0), 4, 64);
    float2 scaledVelStep = input.oVelScale / (float)g_SampleCount;

    float4 result = float4(0.0);

    for (int i = 0; i < g_SampleCount; i++)
    {
        float2 offset = blurAmount * scaledVelStep * (float)i;
        float2 samplePos = input.oTexCoord0 + offset;

        result += texture.sample(samp, samplePos, level(0));
    }

    return result * (1.0 / (float)g_SampleCount);
}
