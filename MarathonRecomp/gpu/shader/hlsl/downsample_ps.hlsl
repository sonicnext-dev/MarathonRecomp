#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#ifdef __spirv__

#define s0_Texture2DDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 0)

#else

cbuffer SharedConstants : register(b2, space4)
{
    uint s0_Texture2DDescriptorIndex : packoffset(c0.x);
    DEFINE_SHARED_CONSTANTS();
};

#endif

float4 shaderMain(
    in float4 oPos : SV_Position,
    in float4 oTexCoord0 : TEXCOORD0) : SV_Target
{
    Texture2D<float4> texture = g_Texture2DDescriptorHeap[s0_Texture2DDescriptorIndex];

    float2 pixelCoord = oTexCoord0.xy * float2(getTexture2DDimensions(texture));
    float2 step = float2(ddx(pixelCoord.x), ddy(pixelCoord.y)) / 4.0;

    float4 color = 0.0;

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
            color += texture.Load(int3(int2(pixelCoord + (float2(x, y) - 1.5) * step), 0));
    }

    return color / 16.0;
}
