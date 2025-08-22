#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>

namespace Sonicteam::SoX::Graphics::Xenon
{
    class TextureXenon : public IResource
    {
    public:
        MARATHON_INSERT_PADDING(0x1C);
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        MARATHON_INSERT_PADDING(4);
    };
}
