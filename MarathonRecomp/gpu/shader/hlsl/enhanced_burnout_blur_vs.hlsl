#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#ifdef __spirv__

#define g_Velocity vk::RawBufferLoad<float4>(g_PushConstants.VertexShaderConstants + 3360, 0x10)

#else

cbuffer VertexShaderConstants : register(b1, space4)
{
    float4 g_Velocity : packoffset(c210);
};

#endif

void shaderMain(
    [[vk::location(0)]] in float4 iPosition0 : POSITION0,
    [[vk::location(13)]] in float2 iTexCoord0 : TEXCOORD0,
    out float4 oPos : SV_Position,
    out float2 oTexCoord0 : TEXCOORD0,
    out float2 oVelocity : TEXCOORD1,
    out float2 oVelScale : TEXCOORD2)
{
    oPos = iPosition0;
    oTexCoord0 = iTexCoord0;

    float2 centeredUV;
    centeredUV.x = iTexCoord0.y * 2.0 - 1.0;
    centeredUV.y = iTexCoord0.x * 2.0 - 1.0;

    oVelocity.x = -g_Velocity.x - centeredUV.y;
    oVelocity.y = centeredUV.x - g_Velocity.y;

    float2 scaledVec = oVelocity * g_Velocity.w;
    oVelScale.x = scaledVec.x * 0.00002;
    oVelScale.y = -scaledVec.y * 0.00001;
}
