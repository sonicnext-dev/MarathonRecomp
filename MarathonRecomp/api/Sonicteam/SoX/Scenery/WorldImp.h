#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Scenery/World.h>

namespace Sonicteam::SoX::Scenery
{
    class WorldImp : public World
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(0x78);
        };

        xpointer<Vftable> m_pVftable;
        MARATHON_INSERT_PADDING(0x16C);
    };

    MARATHON_ASSERT_SIZEOF(WorldImp, 0x170);
}
