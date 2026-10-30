#pragma once

#include "video.h"

enum class RenderCommandType
{
    SetRenderState,
    DestructResource,
    UnlockTextureRect,
    UnlockBuffer16,
    UnlockBuffer32,
    DrawImGui,
    ExecuteCommandList,
    BeginCommandList,
    StretchRect,
    SetRenderTarget,
    SetDepthStencilSurface,
    ExecutePendingStretchRectCommands,
    Clear,
    SetViewport,
    SetTexture,
    SetScissorRect,
    SetSamplerState,
    SetBooleans,
    SetVertexShaderConstants,
    SetPixelShaderConstants,
    AddPipeline,
    DrawPrimitive,
    DrawIndexedPrimitive,
    DrawPrimitiveUP,
    SetVertexDeclaration,
    SetVertexShader,
    SetStreamSource,
    SetIndices,
    SetPixelShader,
    SetConditionalSurvey,
    SetConditionalRendering,
};

struct RenderCommand
{
    RenderCommandType type;
    union
    {
        struct
        {
            GuestRenderState type;
            uint32_t value;
        } setRenderState;

        struct
        {
            GuestResource* resource;
        } destructResource;

        struct
        {
            GuestTexture* texture;
        } unlockTextureRect;

        struct
        {
            GuestBuffer* buffer;
        } unlockBuffer;

        struct
        {
            GuestDevice* device;
            uint32_t flags;
            GuestTexture* texture;
            uint32_t destSliceOrFace;
        } stretchRect;

        struct
        {
            GuestSurface* renderTarget;
        } setRenderTarget;

        struct
        {
            GuestSurface* depthStencil;
        } setDepthStencilSurface;

        struct
        {
            uint32_t flags;
            float color[4];
            float z;
            uint32_t stencil;
        } clear;

        struct
        {
            float x;
            float y;
            float width;
            float height;
            float minDepth;
            float maxDepth;
        } setViewport;

        struct
        {
            uint32_t index;
            GuestTexture* texture;
        } setTexture;

        struct
        {
            int32_t left;
            int32_t top;
            int32_t right;
            int32_t bottom;
        } setScissorRect;

        struct
        {
            uint32_t index;
            uint32_t data0;
            uint32_t data3;
            uint32_t data5;
        } setSamplerState;

        struct
        {
            uint32_t booleans;
        } setBooleans;

        struct
        {
            uint8_t* memory;
            uint32_t index;
            uint32_t size;
        } setVertexShaderConstants;

        struct
        {
            uint8_t* memory;
            uint32_t index;
            uint32_t size;
        } setPixelShaderConstants;

        struct
        {
            XXH64_hash_t hash;
            RenderPipeline* pipeline;
        } addPipeline;

        struct
        {
            uint32_t primitiveType;
            uint32_t startVertex;
            uint32_t primitiveCount;
        } drawPrimitive;

        struct
        {
            uint32_t primitiveType;
            int32_t baseVertexIndex;
            uint32_t startIndex;
            uint32_t primCount;
        } drawIndexedPrimitive;

        struct
        {
            uint32_t primitiveType;
            uint32_t primitiveCount;
            uint8_t* vertexStreamZeroData;
            uint32_t vertexStreamZeroSize;
            uint32_t vertexStreamZeroStride;
        } drawPrimitiveUP;

        struct
        {
            GuestVertexDeclaration* vertexDeclaration;
        } setVertexDeclaration;

        struct
        {
            GuestShader* shader;
        } setVertexShader;

        struct
        {
            uint32_t index;
            GuestBuffer* buffer;
            uint32_t offset;
            uint32_t stride;
        } setStreamSource;

        struct
        {
            GuestBuffer* buffer;
        } setIndices;

        struct
        {
            GuestShader* shader;
        } setPixelShader;

        struct
        {
            bool enabled;
            uint32_t index;
        } setConditionalSurvey;

        struct
        {
            bool enabled;
            uint32_t index;
        } setConditionalRendering;
    };
};

extern moodycamel::BlockingConcurrentQueue<RenderCommand> g_renderQueue;
