#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>
#include <Sonicteam/SoX/Graphics/Texture.h>

namespace Sonicteam::SoX::Graphics
{
    class Surface : public IResource
    {
    public:
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        MARATHON_INSERT_PADDING(4);
        xpointer<void> m_pGuestSurface;
        xpointer<Texture> m_spTexture; // RefSharedPointer but st didnt use seems their auto type for it (why they did this manually)
        xpointer<void> m_pGuestTexture;
    };
}
