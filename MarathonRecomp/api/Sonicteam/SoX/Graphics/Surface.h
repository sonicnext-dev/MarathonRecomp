#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

namespace Sonicteam::SoX::Graphics
{
    class Texture;

    class Surface : public IResource
    {
    public:
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        MARATHON_INSERT_PADDING(4);
        xpointer<void> m_pSurface;
        RefSharedPointer<Texture> m_spTexture;
        xpointer<void> m_pTexture;
        MARATHON_INSERT_PADDING(0x0C);
    };

    MARATHON_ASSERT_OFFSETOF(Surface, m_Width, 0x64);
    MARATHON_ASSERT_OFFSETOF(Surface, m_Height, 0x68);
    MARATHON_ASSERT_OFFSETOF(Surface, m_pSurface, 0x70);
    MARATHON_ASSERT_OFFSETOF(Surface, m_spTexture, 0x74);
    MARATHON_ASSERT_OFFSETOF(Surface, m_pTexture, 0x78);
    MARATHON_ASSERT_SIZEOF(Surface, 0x88);
}
