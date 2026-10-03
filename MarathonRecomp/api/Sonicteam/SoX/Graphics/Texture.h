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
        Array<RefSharedPointer<Surface>, 6> m_aspSurfaces;
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        xpointer<Device> m_pDevice;
    };

    MARATHON_ASSERT_OFFSETOF(Texture, m_pTexture, 0x64);
    MARATHON_ASSERT_OFFSETOF(Texture, m_aspSurfaces, 0x68);
    MARATHON_ASSERT_OFFSETOF(Texture, m_Width, 0x80);
    MARATHON_ASSERT_OFFSETOF(Texture, m_Height, 0x84);
    MARATHON_ASSERT_OFFSETOF(Texture, m_pDevice, 0x88);
    MARATHON_ASSERT_SIZEOF(Texture, 0x8C);
}
