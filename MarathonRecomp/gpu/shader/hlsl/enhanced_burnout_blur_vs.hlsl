#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#ifdef __spirv__

#define g_Velocity = vk::RawBufferLoad<float4>(g_PushConstants.VertexShaderConstants + 210, 0x10);
#define g_SampleCount vk::RawBufferLoad<int>(g_PushConstants.SharedConstants + 4)

#else

cbuffer VertexShaderConstants : register(b1, space4)
{
    float4 g_Velocity : packoffset(c210);
};

cbuffer SharedConstants : register(b2, space4)
{
    int g_SampleCount : packoffset(c0.y);
};

#endif

void shaderMain(
    [[vk::location(0)]] in float4 iPosition0 : POSITION0,
    [[vk::location(8)]] in float2 iTexCoord0 : TEXCOORD0,
    out float4 oPos : SV_Position,
    out float2 oTexCoord0 : TEXCOORD0,
    out float2 oVelocity : TEXCOORD1,
    out float2 oVelScale : TEXCOORD2)
{
    oPos = iPosition0;
    oTexCoord0 = iTexCoord0;

    if (g_SampleCount == 1)
    {
        oVelocity = float2(0.0);
        oVelScale = float2(0.0);
        return;
    }

    float2 centeredUV = iTexCoord0 * 2.0 - 1.0;

    oVelocity.x = -g_Velocity.x - centeredUV.y;
    oVelocity.y = centeredUV.x - centeredUV.y;

    float sampleScaleX = 0.00002 / (float)g_SampleCount;
    float sampleScaleY = 0.00001 / (float)g_SampleCount;

    float2 scaledVec = oVelocity * g_Velocity.w;
    output oVelScale.x = scaledVec * sampleScaleX;
    output oVelScale.y = scaledVec * sampleScaleY;
}
