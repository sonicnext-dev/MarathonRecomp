#include "../../../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#ifdef __spirv__

#define s0_Texture2DDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 0)
#define s0_SamplerDescriptorIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 192)
#define g_DepthFadeParams vk::RawBufferLoad<float4>(g_PushConstants.SharedConstants + 320)
#define g_DepthFadeTextureIndex vk::RawBufferLoad<uint>(g_PushConstants.SharedConstants + 336)

#else

cbuffer SharedConstants : register(b2, space4)
{
	uint s0_Texture2DDescriptorIndex : packoffset(c0.x);
	uint s0_SamplerDescriptorIndex : packoffset(c12.x);
	DEFINE_SHARED_CONSTANTS();
	float4 g_DepthFadeParams : packoffset(c20);
	uint g_DepthFadeTextureIndex : packoffset(c21.x);
};

#endif

struct Interpolators
{
	float4 iPos : SV_Position;
	float4 iTexCoord0 : TEXCOORD0;
	float4 iTexCoord1 : TEXCOORD1;
};

float4 shaderMain(Interpolators input) : SV_Target0
{
	float4 color = tfetch2D(s0_Texture2DDescriptorIndex, s0_SamplerDescriptorIndex, input.iTexCoord1.xy, float2(0, 0)) * input.iTexCoord0;

	float4 depthFadeParams = g_DepthFadeParams;
	float sceneDepth = g_Texture2DDescriptorHeap[g_DepthFadeTextureIndex].Load(int3(input.iPos.xy, 0)).x;
	float sceneZ = depthFadeParams.y / (sceneDepth - depthFadeParams.x);
	float fragmentZ = depthFadeParams.y / (input.iPos.z - depthFadeParams.x);
	// https://developer.download.nvidia.com/whitepapers/2007/SDK10/SoftParticles_hi.pdf
	float d = saturate((sceneZ - fragmentZ) * depthFadeParams.z);
    float c = 0.5 * pow(saturate(2.0 * (d > 0.5 ? 1.0 - d : d)), 2.0);
    color.w *= d > 0.5 ? 1.0 - c : c;

	return color;
}
