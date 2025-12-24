#pragma once

#include <Marathon.inl>

enum D3DXBSTENCILOP
{
    D3DXBSTENCILOP_KEEP = 0,
    D3DXBSTENCILOP_ZERO = 1,
    D3DXBSTENCILOP_REPLACE = 2,
    D3DXBSTENCILOP_INCRSAT = 3,
    D3DXBSTENCILOP_DECRSAT = 4,
    D3DXBSTENCILOP_INVERT = 5,
    D3DXBSTENCILOP_INCR = 6,
    D3DXBSTENCILOP_DECR = 7
};

class IDirect3DVertexShader9;
class IDirect3DPixelShader9;
class D3DIndexBuffer;


typedef struct _D3DVERTEXELEMENT9
{
    be<uint16_t>    Stream;     // Stream index
    be<uint16_t>    Offset;     // Offset in the stream in bytes
    be<uint32_t>   Type;       // Data type, e.g. D3DDECLTYPE_FLOAT3
    uint8_t    Method;     // Processing method, e.g. D3DDECLMETHOD_DEFAULT
    uint8_t    Usage;      // Semantics, e.g. D3DDECLUSAGE_POSITION
    uint8_t    UsageIndex; // Semantic index
}   D3DVERTEXELEMENT9, * LPD3DVERTEXELEMENT9;





struct D3DVertexDeclaration
{
    be<uint32_t> Common;
    be<uint32_t> ReferenceCount;
    be<uint32_t> Fence;
    be<uint32_t> ReadFence;
    be<uint32_t> Identifier;
    be<uint32_t> BaseFlush;
};

struct GPUVERTEX_FETCH_CONSTANT
{
    MARATHON_INSERT_PADDING(8);
};

struct D3DVertexBuffer
{
    be<uint32_t> Common;
    be<uint32_t> ReferenceCount;
    be<uint32_t> Fence;
    be<uint32_t> ReadFence;
    be<uint32_t> Identifier;
    be<uint32_t> BaseFlush;
    GPUVERTEX_FETCH_CONSTANT Format;
};

typedef be<uint32_t> D3DCOLOR;

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define D3DCOLOR_COLORVALUE(r,g,b,a) \
    D3DCOLOR_RGBA((uint32_t)((r)*255.f),(uint32_t)((g)*255.f),(uint32_t)((b)*255.f),(uint32_t)((a)*255.f))

#define D3DCOLOR_RGBA(r,g,b,a) D3DCOLOR_ARGB(a,r,g,b)

#define D3DCOLOR_XRGB(r,g,b) D3DCOLOR_ARGB(0xff,r,g,b)

#define D3DCOLOR_XYUV(y,u,v) D3DCOLOR_ARGB(0xff,y,u,v)

typedef struct _D3DCOLORVALUE 
{
    be<float> r;
    be<float> g;
    be<float> b;
    be<float> a;
} D3DCOLORVALUE;

#define _D3DDECLUSAGE GuestDeclUsage
#define _D3DDECLMETHOD GuestDeclMethod
#define _D3DDECLTYPE GuestDeclType
