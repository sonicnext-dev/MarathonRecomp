#include "render_command.h"

#include <kernel/function.h>
#include <kernel/memory.h>
#include <user/config.h>

static void SetTexture(GuestDevice* device, uint32_t index, GuestTexture* texture)
{
    if (Config::GetPreferredControllerIcons() == EControllerIcons::PlayStation &&
        texture != nullptr && texture->patchedTexture != nullptr)
    {
        texture = texture->patchedTexture.get();
    }

    g_renderQueue.enqueue(RenderCommands::SetTexture
    {
        .index = index, .texture = texture
    });
}

static void SetScissorRect(GuestDevice* device, GuestRect* rect)
{
    g_renderQueue.enqueue(RenderCommands::SetScissorRect
    {
        .left = rect->left,
        .top = rect->top,
        .right = rect->right,
        .bottom = rect->bottom
    });
}

static void SetVertexDeclaration(GuestDevice* device, GuestVertexDeclaration* vertexDeclaration)
{
    g_renderQueue.enqueue(RenderCommands::SetVertexDeclaration { vertexDeclaration });
    device->vertexDeclaration = g_memory.MapVirtual(vertexDeclaration);
}

static void SetVertexShader(GuestDevice* device, GuestShader* shader)
{
    g_renderQueue.enqueue(RenderCommands::SetVertexShader { shader });
}

static void SetStreamSource(GuestDevice* device, uint32_t index, GuestBuffer* buffer, uint32_t offset, uint32_t stride)
{
    g_renderQueue.enqueue(RenderCommands::SetStreamSource
    {
        .index = index,
        .buffer = buffer,
        .offset = offset,
        .stride = stride
    });
}

static void SetIndices(GuestDevice* device, GuestBuffer* buffer)
{
    g_renderQueue.enqueue(RenderCommands::SetIndices { buffer });
}

static void SetPixelShader(GuestDevice* device, GuestShader* shader)
{
    g_renderQueue.enqueue(RenderCommands::SetPixelShader { shader });
}

static void BeginConditionalSurvey(GuestDevice* device, uint32_t index)
{
    assert(index < CONDITIONAL_SURVEY_MAX && "Invalid conditional survey index.");

    g_renderQueue.enqueue(RenderCommands::SetConditionalSurvey
    {
        .enabled = true, .index = index
    });
}

static void EndConditionalSurvey(GuestDevice* device)
{
    g_renderQueue.enqueue(RenderCommands::SetConditionalSurvey
    {
        .enabled = false, .index = 0
    });
}

static void BeginConditionalRendering(GuestDevice* device, uint32_t index)
{
    assert(index < CONDITIONAL_SURVEY_MAX && "Invalid conditional rendering index.");

    g_renderQueue.enqueue(RenderCommands::SetConditionalRendering
    {
        .enabled = true, .index = index
    });
}

static void EndConditionalRendering(GuestDevice* device)
{
    g_renderQueue.enqueue(RenderCommands::SetConditionalRendering
    {
        .enabled = false, .index = 0
    });
}

static void SetClipPlane(GuestDevice* device, uint32_t index, const be<float>* plane)
{
    if (index != 0)
        return;

    g_renderQueue.enqueue(RenderCommands::SetClipPlane
    {
        .plane = { plane[0].get(), plane[1].get(), plane[2].get(), plane[3].get() }
    });
}

GUEST_FUNCTION_HOOK(sub_8253AC40, SetTexture);
GUEST_FUNCTION_HOOK(sub_82543628, SetScissorRect);

GUEST_FUNCTION_HOOK(sub_825470F8, SetVertexDeclaration);
GUEST_FUNCTION_HOOK(sub_82546EE0, SetVertexShader);
GUEST_FUNCTION_HOOK(sub_82546BD8, SetPixelShader);

GUEST_FUNCTION_HOOK(sub_82543918, SetStreamSource);
GUEST_FUNCTION_HOOK(sub_82543AC8, SetIndices);

GUEST_FUNCTION_HOOK(sub_82636BF8, BeginConditionalSurvey);
GUEST_FUNCTION_HOOK(sub_82636C08, EndConditionalSurvey);
GUEST_FUNCTION_HOOK(sub_82636C10, BeginConditionalRendering);
GUEST_FUNCTION_HOOK(sub_82636C18, EndConditionalRendering);

GUEST_FUNCTION_HOOK(sub_82543CF0, SetClipPlane);
