#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Scenery/Drawable.h>

namespace Sonicteam::SoX::Scenery
{
    class Clump : public Drawable
    {
    public:
        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_SIZEOF(Clump, 0x80);
}
