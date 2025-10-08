#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Scenery/Drawable.h>

namespace Sonicteam
{
    class MercuryDrawable : public SoX::Scenery::Drawable
    {
        MARATHON_INSERT_PADDING(0xF0);
    };
    
    MARATHON_ASSERT_SIZEOF(MercuryDrawable, 0x160);
}
