#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/Graphics/Surface.h>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/SoX/Graphics/Device.h>
#include <gpu/video.h>

namespace Sonicteam::SoX::Graphics
{
    class Texture : public IResource
    {
    public:
        xpointer<GuestTexture> m_pTexture;
        SoX::Array<SoX::RefSharedPointer<SoX::Graphics::Surface>, 6> m_aspSurface;
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        xpointer<SoX::Graphics::Device> m_pDevice;
    };
}
