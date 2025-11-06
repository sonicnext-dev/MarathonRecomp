#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/Array.h>
#include <gpu/video.h>

namespace Sonicteam::SoX::Graphics
{
    class Surface;
    class Device;

    class Texture : public IResource
    {
    public:
        xpointer<GuestTexture> m_pTexture;
        Array<RefSharedPointer<Surface>, 6> m_aspSurface;
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        xpointer<Device> m_pDevice;
    };
}
