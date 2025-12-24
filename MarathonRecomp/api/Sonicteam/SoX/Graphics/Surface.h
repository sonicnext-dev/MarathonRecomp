#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

namespace Sonicteam::SoX::Graphics
{
    class Texture;

    class Surface : public Sonicteam::SoX::IResource
    {
    public:
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        MARATHON_INSERT_PADDING(4);
        xpointer<void> m_pSurface;
        xpointer<Texture> m_spTexture; // RefSharedPointer
        xpointer<void> m_pTexture;
        MARATHON_INSERT_PADDING(0xC);
    };
}