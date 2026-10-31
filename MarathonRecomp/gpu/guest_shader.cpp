#include "guest_shader.h"

#include <kernel/function.h>
#include <kernel/heap.h>
#include <shader/shader_cache.h>

static ShaderCacheEntry* FindShaderCacheEntry(XXH64_hash_t hash)
{
    auto end = g_shaderCacheEntries + g_shaderCacheEntryCount;
    auto findResult = std::lower_bound(g_shaderCacheEntries, end, hash, [](ShaderCacheEntry& lhs, XXH64_hash_t rhs)
        {
            return lhs.hash < rhs;
        });

    return findResult != end && findResult->hash == hash ? findResult : nullptr;
}

GuestShader* GetOrCreateShader(XXH64_hash_t hash, ResourceType resourceType) {
    auto findResult = FindShaderCacheEntry(hash);
    GuestShader* shader = nullptr;

    if (findResult == nullptr)
    {
        LOGF_WARNING("Shader not found by hash: {:x}", hash);
        LOG_WARNING("Perhaps the path to the required shader will be printed before this error");
        __builtin_trap();
    }
    if (findResult != nullptr)
    {
        if (findResult->guestShader == nullptr)
        {
            shader = g_userHeap.AllocPhysical<GuestShader>(resourceType);
            shader->shaderCacheEntry = findResult;
#ifdef ASYNC_PSO_DEBUG
            shader->name = findResult->filename;
#endif

            findResult->guestShader = shader;
        }
        else
        {
            shader = findResult->guestShader;
        }
    }

    if (shader == nullptr)
        shader = g_userHeap.AllocPhysical<GuestShader>(resourceType);
    else
        shader->AddRef();

    return shader;
}

static GuestShader* CreateShader(const be<uint32_t>* function, ResourceType resourceType)
{
    XXH64_hash_t hash = XXH3_64bits(function, function[1] + function[2]);
    return GetOrCreateShader(hash, resourceType);
}

static GuestShader* CreateVertexShader(const be<uint32_t>* function)
{
    return CreateShader(function, ResourceType::VertexShader);
}

static GuestShader* CreatePixelShader(const be<uint32_t>* function)
{
    return CreateShader(function, ResourceType::PixelShader);
}

GUEST_FUNCTION_HOOK(sub_82548700, CreateVertexShader);
GUEST_FUNCTION_HOOK(sub_82548608, CreatePixelShader);
