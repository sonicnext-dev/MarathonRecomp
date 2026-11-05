#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#define s0_Texture2DDescriptorIndex (*(reinterpret_cast<device uint*>(g_PushConstants.SharedConstants + 0)))
#define s0_SamplerDescriptorIndex (*(reinterpret_cast<device uint*>(g_PushConstants.SharedConstants + 192)))
#define g_DepthFadeParams (*(reinterpret_cast<device float4*>(g_PushConstants.SharedConstants + 320)))
#define g_DepthFadeTextureIndex (*(reinterpret_cast<device uint*>(g_PushConstants.SharedConstants + 336)))

struct Interpolators
{
	float4 iPos [[position]];
	float4 iTexCoord0 [[user(TEXCOORD0)]];
	float4 iTexCoord1 [[user(TEXCOORD1)]];
};

[[fragment]]
float4 shaderMain(Interpolators input [[stage_in]],
                  constant Texture2DDescriptorHeap* g_Texture2DDescriptorHeap [[buffer(0)]],
                  constant SamplerDescriptorHeap* g_SamplerDescriptorHeap [[buffer(3)]],
                  constant PushConstants& g_PushConstants [[buffer(8)]])
{
	float4 color = tfetch2D(g_Texture2DDescriptorHeap, g_SamplerDescriptorHeap,
		s0_Texture2DDescriptorIndex, s0_SamplerDescriptorIndex, input.iTexCoord1.xy, float2(0, 0)) * input.iTexCoord0;

	float4 depthFadeParams = g_DepthFadeParams;
	float sceneDepth = g_Texture2DDescriptorHeap[g_DepthFadeTextureIndex].tex.read(uint2(input.iPos.xy)).x;
	float sceneZ = depthFadeParams.y / (sceneDepth - depthFadeParams.x);
	float fragmentZ = depthFadeParams.y / (input.iPos.z - depthFadeParams.x);
	// https://developer.download.nvidia.com/whitepapers/2007/SDK10/SoftParticles_hi.pdf
    float d = saturate((sceneZ - fragmentZ) * depthFadeParams.z);
    float c = 0.5 * pow(saturate(2.0 * (d > 0.5 ? 1.0 - d : d)), 2.0);
    color.w *= d > 0.5 ? 1.0 - c : c;

	return color;
}
