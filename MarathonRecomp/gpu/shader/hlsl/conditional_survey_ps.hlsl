#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#ifndef __spirv__

cbuffer SharedConstants : register(b2, space4)
{
	DEFINE_SHARED_CONSTANTS();
};

#endif

[earlydepthstencil]
float4 shaderMain() : SV_Target
{
    atomicFetchAddUint(g_ConditionalSurveyBuffer, g_conditionalSurveyIndex, 1);
    return float4(0.0, 0.0, 0.0, 0.0);
}
