#pragma once

#include <Marathon.inl>
#include <stdx/string.h>

namespace Sonicteam::SoX::Graphics
{
    enum BufferType : uint32_t
    {
        BufferType_None,
        BufferType_Color,
        BufferType_Depth,
        BufferType_ColorDepth,
        BufferType_Stencil,
        BufferType_5,
        BufferType_DepthStencil,
        BufferType_All = BufferType_Color | BufferType_Depth | BufferType_ColorDepth | BufferType_Stencil | BufferType_5 | BufferType_DepthStencil
    };

    inline BufferType GetBufferTypeByName(const char* in_pName)
    {
        const auto name = guest_stack_var<stdx::string>(in_pName);

        return BufferType(GuestToHostFunction<uint32_t>(sub_82606790, name.get()));
    }
}
