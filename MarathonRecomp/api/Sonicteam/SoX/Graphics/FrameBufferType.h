#pragma once

namespace Sonicteam::SoX::Graphics
{
    enum FrameBufferType : uint32_t
    {
        FrameBufferType_None,
        FrameBufferType_Color,
        FrameBufferType_Depth,
        FrameBufferType_ColorDepth,
        FrameBufferType_Stencil,
        FrameBufferType_5,
        FrameBufferType_DepthStencil,
        FrameBufferType_All = FrameBufferType_Color | FrameBufferType_Depth | FrameBufferType_ColorDepth | FrameBufferType_Stencil | FrameBufferType_5 | FrameBufferType_DepthStencil
    };
}
