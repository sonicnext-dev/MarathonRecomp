#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Scenery/Drawable.h>
#include <Sonicteam/SoX/Graphics/Vertex.h>
#include <Sonicteam/MyGraphicsDevice.h>

namespace Sonicteam
{
    class CObjBalloonIconDrawable : public SoX::Scenery::Drawable
    {
    public:
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
        MARATHON_INSERT_PADDING(0x24);
        SoX::Graphics::Vertex m_Vertices[4]; // BL, TL, BR, TR
    };
}
