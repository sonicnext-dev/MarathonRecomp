#include "vertex_declaration.h"
#include "guest_convert.h"

#include <kernel/function.h>
#include <kernel/heap.h>
#include <xxHashMap.h>

static Mutex g_vertexDeclarationMutex;
static xxHashMap<GuestVertexDeclaration*> g_vertexDeclarations;

GuestVertexDeclaration* CreateVertexDeclarationWithoutAddRef(GuestVertexElement* vertexElements)
{
    size_t vertexElementCount = 0;
    auto vertexElement = vertexElements;

    while (vertexElement->stream != 0xFF && vertexElement->type != D3DDECLTYPE_UNUSED)
    {
        vertexElement->padding = 0;
        ++vertexElement;
        ++vertexElementCount;
    }

    vertexElement->padding = 0; // Clear the padding in D3DDECL_END()

    std::lock_guard lock(g_vertexDeclarationMutex);

    XXH64_hash_t hash = XXH3_64bits(vertexElements, vertexElementCount * sizeof(GuestVertexElement));
    auto& vertexDeclaration = g_vertexDeclarations[hash];

    if (vertexDeclaration == nullptr)
    {
        vertexDeclaration = g_userHeap.AllocPhysical<GuestVertexDeclaration>(ResourceType::VertexDeclaration);
        vertexDeclaration->hash = hash;

        static std::vector<RenderInputElement> inputElements;
        inputElements.clear();

        struct Location
        {
            uint32_t usage;
            uint32_t usageIndex;
            uint32_t location;
        };

        // Should match the locations defined in XenosRecomp.
        constexpr Location locations[] =
        {
            { D3DDECLUSAGE_POSITION, 0, 0 },
            { D3DDECLUSAGE_POSITION, 1, 1 },
            { D3DDECLUSAGE_POSITION, 2, 2 },
            { D3DDECLUSAGE_POSITION, 3, 3 },
            { D3DDECLUSAGE_NORMAL, 0, 4 },
            { D3DDECLUSAGE_NORMAL, 1, 5 },
            { D3DDECLUSAGE_NORMAL, 2, 6 },
            { D3DDECLUSAGE_NORMAL, 3, 7 },
            { D3DDECLUSAGE_TANGENT, 0, 8 },
            { D3DDECLUSAGE_TANGENT, 1, 9 },
            { D3DDECLUSAGE_TANGENT, 2, 10 },
            { D3DDECLUSAGE_TANGENT, 3, 11 },
            { D3DDECLUSAGE_BINORMAL, 0, 12 },
            { D3DDECLUSAGE_TEXCOORD, 0, 13 },
            { D3DDECLUSAGE_TEXCOORD, 1, 14 },
            { D3DDECLUSAGE_TEXCOORD, 2, 15 },
            { D3DDECLUSAGE_TEXCOORD, 3, 16 },
            { D3DDECLUSAGE_COLOR, 0, 17 },
            { D3DDECLUSAGE_BLENDINDICES, 0, 18 },
            { D3DDECLUSAGE_BLENDWEIGHT, 0, 19 },
        };

        vertexElement = vertexElements;
        while (vertexElement->stream != 0xFF && vertexElement->type != D3DDECLTYPE_UNUSED)
        {
            uint32_t resolvedLocation = ~0;
            for (auto& location : locations)
            {
                if (location.usage == vertexElement->usage && location.usageIndex == vertexElement->usageIndex)
                {
                    resolvedLocation = location.location;
                    break;
                }
            }

            if (resolvedLocation == ~0)
            {
                // Bound but not used by any guest shaders.
                ++vertexElement;
                continue;
            }

            auto& inputElement = inputElements.emplace_back();
            inputElement.semanticName = ConvertDeclUsage(vertexElement->usage);
            inputElement.semanticIndex = vertexElement->usageIndex;
            inputElement.location = resolvedLocation;
            inputElement.format = ConvertDeclType(vertexElement->type);
            inputElement.slotIndex = vertexElement->stream;
            inputElement.alignedByteOffset = vertexElement->offset;

            switch (vertexElement->usage)
            {
            case D3DDECLUSAGE_NORMAL:
                switch (vertexElement->type)
                {
                case D3DDECLTYPE_SHORT2:
                case D3DDECLTYPE_SHORT4:
                case D3DDECLTYPE_SHORT2N:
                case D3DDECLTYPE_SHORT4N:
                case D3DDECLTYPE_USHORT2N:
                case D3DDECLTYPE_USHORT4N:
                case D3DDECLTYPE_FLOAT16_2:
                case D3DDECLTYPE_FLOAT16_4:
                    vertexDeclaration->swappedNormals |= 1 << vertexElement->usageIndex;
                    break;
                }

                break;
            case D3DDECLUSAGE_BINORMAL:
                switch (vertexElement->type)
                {
                case D3DDECLTYPE_SHORT2:
                case D3DDECLTYPE_SHORT4:
                case D3DDECLTYPE_SHORT2N:
                case D3DDECLTYPE_SHORT4N:
                case D3DDECLTYPE_USHORT2N:
                case D3DDECLTYPE_USHORT4N:
                case D3DDECLTYPE_FLOAT16_2:
                case D3DDECLTYPE_FLOAT16_4:
                    vertexDeclaration->swappedBinormals |= 1 << vertexElement->usageIndex;
                    break;
                }

                break;
            case D3DDECLUSAGE_TANGENT:
                switch (vertexElement->type)
                {
                case D3DDECLTYPE_SHORT2:
                case D3DDECLTYPE_SHORT4:
                case D3DDECLTYPE_SHORT2N:
                case D3DDECLTYPE_SHORT4N:
                case D3DDECLTYPE_USHORT2N:
                case D3DDECLTYPE_USHORT4N:
                case D3DDECLTYPE_FLOAT16_2:
                case D3DDECLTYPE_FLOAT16_4:
                    vertexDeclaration->swappedTangents |= 1 << vertexElement->usageIndex;
                    break;
                }

                break;
            case D3DDECLUSAGE_BLENDWEIGHT:
                switch (vertexElement->type)
                {
                case D3DDECLTYPE_SHORT2:
                case D3DDECLTYPE_SHORT4:
                case D3DDECLTYPE_SHORT2N:
                case D3DDECLTYPE_SHORT4N:
                case D3DDECLTYPE_USHORT2N:
                case D3DDECLTYPE_USHORT4N:
                case D3DDECLTYPE_FLOAT16_2:
                case D3DDECLTYPE_FLOAT16_4:
                    vertexDeclaration->swappedBlendWeights |= 1 << vertexElement->usageIndex;
                    break;
                }

                break;

            case D3DDECLUSAGE_TEXCOORD:
                switch (vertexElement->type)
                {
                case D3DDECLTYPE_SHORT2:
                case D3DDECLTYPE_SHORT4:
                case D3DDECLTYPE_SHORT2N:
                case D3DDECLTYPE_SHORT4N:
                case D3DDECLTYPE_USHORT2N:
                case D3DDECLTYPE_USHORT4N:
                case D3DDECLTYPE_FLOAT16_2:
                case D3DDECLTYPE_FLOAT16_4:
                    vertexDeclaration->swappedTexcoords |= 1 << vertexElement->usageIndex;
                    break;
                }

                break;
            }

            vertexDeclaration->vertexStreams[vertexElement->stream] = true;

            ++vertexElement;
        }

        auto addInputElement = [&](uint32_t usage, uint32_t usageIndex)
            {
                uint32_t location = ~0;

                for (auto& alsoLocation : locations)
                {
                    if (alsoLocation.usage == usage && alsoLocation.usageIndex == usageIndex)
                    {
                        location = alsoLocation.location;
                        break;
                    }
                }

                assert(location != ~0);

                for (auto& inputElement : inputElements)
                {
                    if (inputElement.location == location)
                        return;
                }

                auto format = RenderFormat::R32_FLOAT;
                switch (usage)
                {
                case D3DDECLUSAGE_NORMAL:
                case D3DDECLUSAGE_TANGENT:
                case D3DDECLUSAGE_BINORMAL:
                case D3DDECLUSAGE_BLENDINDICES:
                    format = RenderFormat::R32G32B32_FLOAT;
                    break;
                }

                inputElements.emplace_back(ConvertDeclUsage(usage), usageIndex, location, format, 15, 0);
            };

        // Assign any unbound usages to null buffer slot.
        for (auto& location : locations)
        {
            addInputElement(location.usage, location.usageIndex);
        }

        vertexDeclaration->inputElements = std::make_unique<RenderInputElement[]>(inputElements.size());
        std::ranges::copy(inputElements, vertexDeclaration->inputElements.get());

        vertexDeclaration->vertexElements = std::make_unique<GuestVertexElement[]>(vertexElementCount + 1);
        std::copy(vertexElements, vertexElements + vertexElementCount + 1, vertexDeclaration->vertexElements.get());

        vertexDeclaration->inputElementCount = uint32_t(inputElements.size());
        vertexDeclaration->vertexElementCount = vertexElementCount + 1;
    }

    vertexDeclaration->AddRef();
    return vertexDeclaration;
}

static GuestVertexDeclaration* CreateVertexDeclaration(GuestVertexElement* vertexElements)
{
    auto vertexDeclaration = CreateVertexDeclarationWithoutAddRef(vertexElements);
    vertexDeclaration->AddRef();
    return vertexDeclaration;
}

GuestVertexDeclaration* FindVertexDeclaration(XXH64_hash_t hash)
{
    std::lock_guard lock(g_vertexDeclarationMutex);
    return g_vertexDeclarations[hash];
}

GUEST_FUNCTION_HOOK(sub_82547118, CreateVertexDeclaration);
