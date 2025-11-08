#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Device.h>
#include <Sonicteam/SoX/Graphics/Surface.h>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/SoX/IResource.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

namespace Sonicteam::SoX::Graphics
{
    class Texture : public IResource
    {
    public:
        xpointer<void> m_pTexture;
        SoX::Array<SoX::RefSharedPointer<Surface>, 6> m_aspSurfaces;
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        xpointer<Device> m_pDevice;
    };
}
