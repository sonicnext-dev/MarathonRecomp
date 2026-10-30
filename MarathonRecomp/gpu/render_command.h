#pragma once

#include "video.h"

#include <variant>

static constexpr uint32_t CONDITIONAL_SURVEY_MAX = 64;

namespace RenderCommands
{
    struct SetRenderState
    {
        GuestRenderState type;
        uint32_t value;
    };

    struct DestructResource
    {
        GuestResource* resource;
    };

    struct UnlockTextureRect
    {
        GuestTexture* texture;
    };

    struct DrawImGui {};

    struct ExecuteCommandList {};

    struct BeginCommandList {};

    struct StretchRect
    {
        uint32_t flags;
        GuestTexture* texture;
        uint32_t destSliceOrFace;
    };

    struct SetRenderTarget
    {
        GuestSurface* renderTarget;
    };

    struct SetDepthStencilSurface
    {
        GuestSurface* depthStencil;
    };

    struct ExecutePendingStretchRectCommands {};

    struct Clear
    {
        uint32_t flags;
        float color[4];
        float z;
        uint32_t stencil;
    };

    struct SetViewport
    {
        float x;
        float y;
        float width;
        float height;
        float minDepth;
        float maxDepth;
    };

    struct SetTexture
    {
        uint32_t index;
        GuestTexture* texture;
    };

    struct SetScissorRect
    {
        int32_t left;
        int32_t top;
        int32_t right;
        int32_t bottom;
    };

    struct SetSamplerState
    {
        uint32_t index;
        uint32_t data0;
        uint32_t data3;
        uint32_t data5;
    };

    struct SetBooleans
    {
        uint32_t booleans;
    };

    struct SetVertexShaderConstants
    {
        uint8_t* memory;
        uint32_t index;
        uint32_t size;
    };

    struct SetPixelShaderConstants
    {
        uint8_t* memory;
        uint32_t index;
        uint32_t size;
    };

    struct AddPipeline
    {
        XXH64_hash_t hash;
        RenderPipeline* pipeline;
    };

    struct DrawPrimitive
    {
        uint32_t primitiveType;
        uint32_t startVertex;
        uint32_t primitiveCount;
    };

    struct DrawIndexedPrimitive
    {
        uint32_t primitiveType;
        int32_t baseVertexIndex;
        uint32_t startIndex;
        uint32_t primCount;
    };

    struct DrawPrimitiveUP
    {
        uint32_t primitiveType;
        uint32_t primitiveCount;
        uint8_t* vertexStreamZeroData;
        uint32_t vertexStreamZeroSize;
        uint32_t vertexStreamZeroStride;
    };

    struct SetVertexDeclaration
    {
        GuestVertexDeclaration* vertexDeclaration;
    };

    struct SetVertexShader
    {
        GuestShader* shader;
    };

    struct SetStreamSource
    {
        uint32_t index;
        GuestBuffer* buffer;
        uint32_t offset;
        uint32_t stride;
    };

    struct SetIndices
    {
        GuestBuffer* buffer;
    };

    struct SetPixelShader
    {
        GuestShader* shader;
    };

    struct SetConditionalSurvey
    {
        bool enabled;
        uint32_t index;
    };

    struct SetConditionalRendering
    {
        bool enabled;
        uint32_t index;
    };

    struct SetClipPlane
    {
        float plane[4];
    };
}

using RenderCommand = std::variant<
    RenderCommands::SetRenderState,
    RenderCommands::DestructResource,
    RenderCommands::UnlockTextureRect,
    RenderCommands::DrawImGui,
    RenderCommands::ExecuteCommandList,
    RenderCommands::BeginCommandList,
    RenderCommands::StretchRect,
    RenderCommands::SetRenderTarget,
    RenderCommands::SetDepthStencilSurface,
    RenderCommands::ExecutePendingStretchRectCommands,
    RenderCommands::Clear,
    RenderCommands::SetViewport,
    RenderCommands::SetTexture,
    RenderCommands::SetScissorRect,
    RenderCommands::SetSamplerState,
    RenderCommands::SetBooleans,
    RenderCommands::SetVertexShaderConstants,
    RenderCommands::SetPixelShaderConstants,
    RenderCommands::AddPipeline,
    RenderCommands::DrawPrimitive,
    RenderCommands::DrawIndexedPrimitive,
    RenderCommands::DrawPrimitiveUP,
    RenderCommands::SetVertexDeclaration,
    RenderCommands::SetVertexShader,
    RenderCommands::SetStreamSource,
    RenderCommands::SetIndices,
    RenderCommands::SetPixelShader,
    RenderCommands::SetConditionalSurvey,
    RenderCommands::SetConditionalRendering,
    RenderCommands::SetClipPlane>;

extern moodycamel::BlockingConcurrentQueue<RenderCommand> g_renderQueue;
