#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

// #define g_SampleCount (*(reinterpret_cast<device int*>(g_PushConstants.VertexShaderConstants + 0)))
#define g_Velocity (*(reinterpret_cast<device float4*>(g_PushConstants.VertexShaderConstants + 3360)))

struct VertexShaderInput
{
    float4 iPosition0 [[attribute(0)]];
    float2 iTexCoord0 [[attribute(13)]];
};

struct Interpolators
{
    float4 oPos [[position]];
    float2 oTexCoord0 [[user(TEXCOORD0)]];
    float2 oVelocity [[user(TEXCOORD1)]];
    float2 oVelScale [[user(TEXCOORD2)]];
};

[[vertex]]
Interpolators shaderMain(VertexShaderInput input [[stage_in]],
                         constant PushConstants& g_PushConstants [[buffer(8)]])
{
    Interpolators output;

    output.oPos = input.iPosition0;
    output.oTexCoord0 = input.iTexCoord0;

    float2 centeredUV;
    centeredUV.x = input.iTexCoord0.y * 2.0 - 1.0;
    centeredUV.y = input.iTexCoord0.x * 2.0 - 1.0;

    output.oVelocity.x = -g_Velocity.x - centeredUV.y;
    output.oVelocity.y = centeredUV.x - g_Velocity.y;

    float2 scaledVec = output.oVelocity * g_Velocity.w;
    output.oVelScale.x = scaledVec.x * 0.00002;
    output.oVelScale.y = -scaledVec.y * 0.00001;

    return output;
}
