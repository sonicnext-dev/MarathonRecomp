#include "pipeline.h"
#include "guest_shader.h"
#include "video_internal.h"

#include <shader/shader_cache.h>

#ifdef ASYNC_PSO_DEBUG
std::atomic<uint32_t> g_pipelinesCurrentlyCompiling;
#endif

XXH64_hash_t GetPipelineStateHash(const PipelineState& pipelineState)
{
    XXH64_hash_t seed = 0;

    // If shaders need to be replaced, make sure they result in a different pipeline hash.
    struct {
        RenderShader* vertex;
        RenderShader* pixel;
    } replacements(GetReplacementShader(pipelineState.vertexShader), GetReplacementShader(pipelineState.pixelShader));
    if (replacements.vertex != nullptr || replacements.pixel != nullptr)
        seed = XXH3_64bits(&replacements, sizeof(replacements));

    return XXH3_64bits_withSeed(&pipelineState, sizeof(pipelineState), seed);
}

void SanitizePipelineState(PipelineState& pipelineState)
{
    if (!pipelineState.zEnable && !pipelineState.stencilEnable)
    {
        pipelineState.depthStencilFormat = RenderFormat::UNKNOWN;
    }

    if (!pipelineState.zEnable)
    {
        pipelineState.zWriteEnable = false;
        pipelineState.zFunc = RenderComparisonFunction::LESS;
        pipelineState.slopeScaledDepthBias = 0.0f;
        pipelineState.depthBias = 0;
    }

    if (!pipelineState.stencilEnable)
    {
        pipelineState.stencilTwoSided = false;
        pipelineState.stencilFunc = RenderComparisonFunction::ALWAYS;
        pipelineState.stencilFail = RenderStencilOp::KEEP;
        pipelineState.stencilZFail = RenderStencilOp::KEEP;
        pipelineState.stencilPass = RenderStencilOp::KEEP;
        pipelineState.stencilMask = 0xFFFFFFFF;
        pipelineState.stencilWriteMask = 0xFFFFFFFF;
        pipelineState.stencilRef = 0;
    }

    if (!pipelineState.stencilTwoSided)
    {
        pipelineState.stencilFuncCCW = pipelineState.stencilFunc;
        pipelineState.stencilFailCCW = pipelineState.stencilFail;
        pipelineState.stencilZFailCCW = pipelineState.stencilZFail;
        pipelineState.stencilPassCCW = pipelineState.stencilPass;
    }

    if (pipelineState.slopeScaledDepthBias == 0.0f)
        pipelineState.slopeScaledDepthBias = 0.0f; // Remove sign.

    if (!pipelineState.colorWriteEnable)
    {
        pipelineState.alphaBlendEnable = false;
        pipelineState.renderTargetFormat = RenderFormat::UNKNOWN;
    }

    if (!pipelineState.alphaBlendEnable)
    {
        pipelineState.srcBlend = RenderBlend::ONE;
        pipelineState.destBlend = RenderBlend::ZERO;
        pipelineState.blendOp = RenderBlendOperation::ADD;
        pipelineState.srcBlendAlpha = RenderBlend::ONE;
        pipelineState.destBlendAlpha = RenderBlend::ZERO;
        pipelineState.blendOpAlpha = RenderBlendOperation::ADD;
    }

    for (size_t i = 0; i < 16; i++)
    {
        if (!pipelineState.vertexDeclaration->vertexStreams[i])
            pipelineState.vertexStrides[i] = 0;
    }

    uint32_t specConstantsMask = 0;
    if (pipelineState.vertexShader->shaderCacheEntry != nullptr)
        specConstantsMask |= pipelineState.vertexShader->shaderCacheEntry->specConstantsMask;

    if (pipelineState.pixelShader != nullptr && pipelineState.pixelShader->shaderCacheEntry != nullptr)
        specConstantsMask |= pipelineState.pixelShader->shaderCacheEntry->specConstantsMask;

    pipelineState.specConstants &= specConstantsMask;
}

std::unique_ptr<RenderPipeline> CreateGraphicsPipeline(const PipelineState& pipelineState)
{
#ifdef ASYNC_PSO_DEBUG
    ++g_pipelinesCurrentlyCompiling;
#endif

    RenderGraphicsPipelineDesc desc;
    desc.pipelineLayout = g_pipelineLayout.get();
    desc.vertexShader = GetOrLinkShader(pipelineState.vertexShader, pipelineState.specConstants);
    if (pipelineState.enableConditionalSurvey)
        desc.pixelShader = GetOrLinkShader(g_conditionalSurveyPSShader.get(), pipelineState.specConstants);
    else if (pipelineState.pixelShader != nullptr)
        desc.pixelShader = GetOrLinkShader(pipelineState.pixelShader, pipelineState.specConstants);
    else
        desc.pixelShader = nullptr;
    desc.depthFunction = pipelineState.zFunc;
    desc.depthEnabled = pipelineState.zEnable;
    desc.depthWriteEnabled = pipelineState.zWriteEnable;
    desc.depthBias = pipelineState.depthBias;
    desc.stencilEnabled = pipelineState.stencilEnable;
    desc.stencilReadMask = pipelineState.stencilMask;
    desc.stencilWriteMask = pipelineState.stencilWriteMask;
    desc.stencilReference = pipelineState.stencilRef;
    desc.stencilFrontFace.compareFunction = pipelineState.stencilFunc;
    desc.stencilFrontFace.failOp = pipelineState.stencilFail;
    desc.stencilFrontFace.depthFailOp = pipelineState.stencilZFail;
    desc.stencilFrontFace.passOp = pipelineState.stencilPass;
    if (pipelineState.stencilTwoSided) {
        desc.stencilBackFace.compareFunction = pipelineState.stencilFuncCCW;
        desc.stencilBackFace.failOp = pipelineState.stencilFailCCW;
        desc.stencilBackFace.depthFailOp = pipelineState.stencilZFailCCW;
        desc.stencilBackFace.passOp = pipelineState.stencilPassCCW;
    } else {
        desc.stencilBackFace = desc.stencilFrontFace;
    }
    desc.slopeScaledDepthBias = pipelineState.slopeScaledDepthBias;
    desc.dynamicDepthBiasEnabled = Video::GetRenderDevice()->getCapabilities().dynamicDepthBias;
    desc.depthClipEnabled = true;
    desc.primitiveTopology = pipelineState.primitiveTopology;
    if (!Video::GetRenderDevice()->getCapabilities().triangleFan && desc.primitiveTopology == RenderPrimitiveTopology::TRIANGLE_FAN) {
        desc.primitiveTopology = RenderPrimitiveTopology::TRIANGLE_LIST;
    }
    desc.cullMode = pipelineState.cullMode;
    desc.frontFace = pipelineState.frontFace;
    desc.renderTargetFormat[0] = pipelineState.renderTargetFormat;
    desc.renderTargetBlend[0].blendEnabled = pipelineState.alphaBlendEnable;
    desc.renderTargetBlend[0].srcBlend = pipelineState.srcBlend;
    desc.renderTargetBlend[0].dstBlend = pipelineState.destBlend;
    desc.renderTargetBlend[0].blendOp = pipelineState.blendOp;
    desc.renderTargetBlend[0].srcBlendAlpha = pipelineState.srcBlendAlpha;
    desc.renderTargetBlend[0].dstBlendAlpha = pipelineState.destBlendAlpha;
    desc.renderTargetBlend[0].blendOpAlpha = pipelineState.blendOpAlpha;
    desc.renderTargetBlend[0].renderTargetWriteMask = pipelineState.colorWriteEnable;
    desc.renderTargetCount = pipelineState.renderTargetFormat != RenderFormat::UNKNOWN ? 1 : 0;
    desc.depthTargetFormat = pipelineState.depthStencilFormat;
    desc.multisampling.sampleCount = pipelineState.sampleCount;
    desc.alphaToCoverageEnabled = pipelineState.enableAlphaToCoverage;
    desc.inputElements = pipelineState.vertexDeclaration->inputElements.get();
    desc.inputElementsCount = pipelineState.vertexDeclaration->inputElementCount;

    RenderSpecConstant specConstant{};
    specConstant.value = pipelineState.specConstants;

    if (pipelineState.specConstants != 0)
    {
        desc.specConstants = &specConstant;
        desc.specConstantsCount = 1;
    }

    RenderInputSlot inputSlots[16]{};
    uint32_t inputSlotIndices[16]{};
    uint32_t inputSlotCount = 0;

    for (size_t i = 0; i < pipelineState.vertexDeclaration->inputElementCount; i++)
    {
        auto& inputElement = pipelineState.vertexDeclaration->inputElements[i];
        auto& inputSlotIndex = inputSlotIndices[inputElement.slotIndex];

        if (inputSlotIndex == NULL)
            inputSlotIndex = ++inputSlotCount;

        auto& inputSlot = inputSlots[inputSlotIndex - 1];
        inputSlot.index = inputElement.slotIndex;
        inputSlot.stride = pipelineState.vertexStrides[inputElement.slotIndex];
        inputSlot.classification = RenderInputSlotClassification::PER_VERTEX_DATA;
    }

    desc.inputSlots = inputSlots;
    desc.inputSlotsCount = inputSlotCount;

    auto pipeline = Video::GetRenderDevice()->createGraphicsPipeline(desc);

#ifdef ASYNC_PSO_DEBUG
    --g_pipelinesCurrentlyCompiling;
#endif

    return pipeline;
}
