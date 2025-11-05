#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Scenery/Drawable.h>
#include <Sonicteam/MyGraphicsDevice.h>

namespace Sonicteam
{
    class CObjBalloonIconDrawable : public SoX::Scenery::Drawable
    {
    public:
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
        MARATHON_INSERT_PADDING(0x24);
        SoX::Graphics::Vertex m_aVertices[4]; // BL, TL, BR, TR
        MARATHON_INSERT_PADDING(0x18);
    };

    MARATHON_ASSERT_OFFSETOF(CObjBalloonIconDrawable, m_pMyGraphicsDevice, 0x70);
    MARATHON_ASSERT_OFFSETOF(CObjBalloonIconDrawable, m_aVertices, 0x98);
    MARATHON_ASSERT_SIZEOF(CObjBalloonIconDrawable, 0x1A0);
}
