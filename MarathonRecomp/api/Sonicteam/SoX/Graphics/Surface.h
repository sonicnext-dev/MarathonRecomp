#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>
#include <Sonicteam/SoX/IResource.h>
#include <gpu/video.h>
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
        xpointer<GuestSurface> m_pGuestSurface; //backbuffer. and any
        xpointer<Texture> m_spTexture; //RefSharedPointer but st didnt use seems their auto type for it (why they did this manually)
        xpointer<GuestTexture> m_pGuestTexture;
        MARATHON_INSERT_PADDING(0xC);
    };
}
