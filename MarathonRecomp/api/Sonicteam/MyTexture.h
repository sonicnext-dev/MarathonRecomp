#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Xenon/TextureXenon.h>

namespace Sonicteam
{
    class MyTexture : public SoX::Graphics::Xenon::TextureXenon
    {
    public:
        MARATHON_INSERT_PADDING(4);
    };

    MARATHON_ASSERT_SIZEOF(MyTexture, 0x90);
}
