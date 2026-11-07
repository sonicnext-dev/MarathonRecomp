#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#ifdef __spirv__

#define s0_Texture2DDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 0)
#define s0_SamplerDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 192)

#else

cbuffer SharedConstants : register(b2, space4)
{
    uint s0_Texture2DDescriptorIndex : packoffset(c0.x);
    uint s0_SamplerDescriptorIndex : packoffset(c12.x);
    DEFINE_SHARED_CONSTANTS();
};

#endif

float4 shaderMain(
    in float4 oPos : SV_Position,
    in float4 oTexCoord0 : TEXCOORD0) : SV_Target
{
    Texture2D<float4> texture = g_Texture2DDescriptorHeap[s0_Texture2DDescriptorIndex];
    SamplerState samplerState = g_SamplerDescriptorHeap[s0_SamplerDescriptorIndex];

    float2 dimensions = float2(getTexture2DDimensions(texture));
    float2 pixelCoord = oTexCoord0.xy * dimensions;
    float2 texelsPerPixel = float2(ddx(pixelCoord.x), ddy(pixelCoord.y));

    pixelCoord += (texelsPerPixel - 1.0) * 0.5;

    return texture.Sample(samplerState, pixelCoord / dimensions);
}
