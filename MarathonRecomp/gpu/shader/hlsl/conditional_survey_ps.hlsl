#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

[earlydepthstencil]
float4 shaderMain() : SV_Target
{
    atomicFetchAddUint(g_ConditionalSurveyBuffer, g_conditionalSurveyIndex, 1);
    return float4(0.0, 0.0, 0.0, 0.0);
}
