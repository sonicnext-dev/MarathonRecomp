#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Texture.h>
#include <Sonicteam/SoX/RefCountObject.h>

#include <gpu/video.h>
#include <d3dxb.h>


namespace Sonicteam::SoX::Graphics
{
    struct VertexInfo
    {
    public:
        template <typename T>
        T& Get(_D3DDECLUSAGE usage, _D3DVERTEXELEMENT9* Info,uint32_t count)
        {
            uint32_t offset = 0;
            for (int i = 0; i < count; i++)
            {
                if (Info[i].Usage == usage)
                {
                    offset = Info[i].Offset.get();
                }
            }
            return *(T*)((char*)this + offset);
        }

        uint32_t GetTypeSize(_D3DDECLTYPE type)
        {
            switch (type) {
            case D3DDECLTYPE_FLOAT1:    return 4;
            case D3DDECLTYPE_FLOAT2:    return 8;
            case D3DDECLTYPE_FLOAT3:    return 12;
            case D3DDECLTYPE_FLOAT4:    return 16;
            case D3DDECLTYPE_D3DCOLOR:  return 4;
            case D3DDECLTYPE_UBYTE4:    return 4;
            case D3DDECLTYPE_SHORT2:    return 4;
            case D3DDECLTYPE_SHORT4:    return 8;
            case D3DDECLTYPE_UBYTE4N:   return 4;
            case D3DDECLTYPE_SHORT2N:   return 4;
            case D3DDECLTYPE_SHORT4N:   return 8;
            case D3DDECLTYPE_USHORT2N:  return 4;
            case D3DDECLTYPE_USHORT4N:  return 8;
            case D3DDECLTYPE_UDEC3:     return 4;
            case D3DDECLTYPE_DEC3N:     return 4;
            case D3DDECLTYPE_FLOAT16_2: return 4;
            case D3DDECLTYPE_FLOAT16_4: return 8;
            case D3DDECLTYPE_UNUSED:    return 0;
            default: return 0;
            }
        }

		template <typename T>
        T* GetNext(_D3DVERTEXELEMENT9* Info, uint32_t count)
        {
            uint32_t vertexStride = 0;
            for (uint32_t i = 0; i < count; i++) {
                if (Info[i].Type == D3DDECLTYPE_UNUSED) break;

                uint32_t elementSize = GetTypeSize((_D3DDECLTYPE)Info[i].Type.get());
                uint32_t elementEnd = Info[i].Offset + elementSize;
                vertexStride = std::max(vertexStride, elementEnd);
            }

            return (T*)((char*)this + vertexStride);
        }
    };

    //16-bit float
    struct float16
    {
        uint16_t value;
    };

    // Default 06 Vertex Declaration ( for sonic_root at least)
    struct DefaultVertex : VertexInfo
    {
        Sonicteam::SoX::Math::Vector3 position;   
        float16 blendWeight[4];
        uint8_t blendIndices[4];          
        float16 normal[4];
        D3DCOLOR color;                   
        float16 texcoord[2];
    };

    MARATHON_ASSERT_OFFSETOF(DefaultVertex, position,0);
    MARATHON_ASSERT_OFFSETOF(DefaultVertex, blendWeight,0xC);
    MARATHON_ASSERT_OFFSETOF(DefaultVertex, blendIndices,0x14);
    MARATHON_ASSERT_OFFSETOF(DefaultVertex, normal,0x18);
    MARATHON_ASSERT_OFFSETOF(DefaultVertex, color,0x20);
    MARATHON_ASSERT_OFFSETOF(DefaultVertex, texcoord,0x24);
    MARATHON_ASSERT_SIZEOF(DefaultVertex, 0x28);

    class VertexBuffer : public SoX::RefCountObject
    {
    public:
        struct Vftable : RefCountObject::Vftable
        {
            be<uint32_t> fpInitialize;
            be<uint32_t> fpLock;
            be<uint32_t> fpUnlock;
        };

        be<uint32_t> m_VertexIndex; // ? or format? //Vertex Size ? == 0x28 (seems correct)
        be<uint32_t> m_VertexCount;
     

        bool Lock(void** pVertexData)
        {
            auto vft = (Vftable*)m_pVftable.get();
            return GuestToHostFunction<bool>(vft->fpLock, this, pVertexData);
        }

        void Unlock()
        {
            auto vft = (Vftable*)m_pVftable.get();
            GuestToHostFunction<void>(vft->fpUnlock, this);
        }

        template <typename T = VertexInfo>
        bool Lock(T*& pVertexData)  
        {
            auto vft = (Vftable*)m_pVftable.get();
            guest_heap_var<xpointer<void>> ppVertexData;
            auto result = GuestToHostFunction<bool>(vft->fpLock, this, ppVertexData.get());

            if (result && ppVertexData->get() != nullptr) 
            {
                pVertexData = reinterpret_cast<T*>(ppVertexData->get());
            }
            else {
                pVertexData = nullptr;
            }

            return result;
        }
    };

    MARATHON_ASSERT_OFFSETOF(VertexBuffer, m_VertexIndex, 0x8);
    MARATHON_ASSERT_OFFSETOF(VertexBuffer, m_VertexCount, 0xC);
    MARATHON_ASSERT_SIZEOF(VertexBuffer, 0x10);
}
