#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Scenery/Drawable.h>
#include <Sonicteam/MyGraphicsDevice.h>

namespace Sonicteam
{
    class CObjBalloonIconDrawable : public SoX::Scenery::Drawable
    {
    public:
        struct Vertex
        {
            be<float> X;
            be<float> Y;
            MARATHON_INSERT_PADDING(0x10);
            be<uint32_t> Colour; // ARGB8888
            be<float> U;
            be<float> V;
            MARATHON_INSERT_PADDING(0x18);
        };

        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
        MARATHON_INSERT_PADDING(0x24);
        Vertex m_Vertices[4]; // BL, TL, BR, TR
    };
}
