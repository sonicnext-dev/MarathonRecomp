#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

[[fragment]]
[[early_fragment_tests]]
float4 shaderMain(device AtomicUintBuffer* g_ConditionalSurveyBuffer [[buffer(4)]],
                  constant PushConstants& g_PushConstants [[buffer(8)]])
{
    atomicFetchAddUint(g_ConditionalSurveyBuffer, g_conditionalSurveyIndex, 1);
    return float4(0.0, 0.0, 0.0, 0.0);
}
