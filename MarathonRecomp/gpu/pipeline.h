#pragma once

#include "video.h"

#pragma pack(push, 1)
struct PipelineState
{
    GuestShader* vertexShader = nullptr;
    GuestShader* pixelShader = nullptr;
    GuestVertexDeclaration* vertexDeclaration = nullptr;
    bool zEnable = true;
    bool zWriteEnable = true;
    bool stencilEnable = false;
    bool stencilTwoSided = false;
    RenderBlend srcBlend = RenderBlend::ONE;
    RenderBlend destBlend = RenderBlend::ZERO;
    RenderCullMode cullMode = RenderCullMode::NONE;
    RenderFrontFace frontFace = RenderFrontFace::CLOCKWISE;
    RenderComparisonFunction zFunc = RenderComparisonFunction::LESS;
    RenderComparisonFunction stencilFunc = RenderComparisonFunction::ALWAYS;
    RenderStencilOp stencilFail = RenderStencilOp::KEEP;
    RenderStencilOp stencilZFail = RenderStencilOp::KEEP;
    RenderStencilOp stencilPass = RenderStencilOp::KEEP;
    RenderComparisonFunction stencilFuncCCW = RenderComparisonFunction::ALWAYS;
    RenderStencilOp stencilFailCCW = RenderStencilOp::KEEP;
    RenderStencilOp stencilZFailCCW = RenderStencilOp::KEEP;
    RenderStencilOp stencilPassCCW = RenderStencilOp::KEEP;
    uint32_t stencilMask = 0xFFFFFFFF;
    uint32_t stencilWriteMask = 0xFFFFFFFF;
    uint32_t stencilRef = 0;
    bool alphaBlendEnable = false;
    RenderBlendOperation blendOp = RenderBlendOperation::ADD;
    float slopeScaledDepthBias = 0.0f;
    int32_t depthBias = 0;
    RenderBlend srcBlendAlpha = RenderBlend::ONE;
    RenderBlend destBlendAlpha = RenderBlend::ZERO;
    RenderBlendOperation blendOpAlpha = RenderBlendOperation::ADD;
    uint32_t colorWriteEnable = uint32_t(RenderColorWriteEnable::ALL);
    RenderPrimitiveTopology primitiveTopology = RenderPrimitiveTopology::TRIANGLE_LIST;
    uint8_t vertexStrides[16]{};
    RenderFormat renderTargetFormat{};
    RenderFormat depthStencilFormat{};
    RenderSampleCounts sampleCount = RenderSampleCount::COUNT_1;
    bool enableAlphaToCoverage = false;
    bool enableConditionalSurvey = false;
    uint32_t specConstants = 0;
};
#pragma pack(pop)

#ifdef ASYNC_PSO_DEBUG
extern std::atomic<uint32_t> g_pipelinesCurrentlyCompiling;
#endif

XXH64_hash_t GetPipelineStateHash(const PipelineState& pipelineState);
void SanitizePipelineState(PipelineState& pipelineState);
std::unique_ptr<RenderPipeline> CreateGraphicsPipeline(const PipelineState& pipelineState);
