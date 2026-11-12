#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Scenery/Camera.h>
#include <Sonicteam/SoX/Scenery/WorldIntersection.h>

namespace Sonicteam
{
    class WorldIntersectionCSM : public SoX::Scenery::WorldIntersection
    {
    public:
        MARATHON_INSERT_PADDING(0x10);
        xpointer<SoX::Scenery::Camera> m_pCamera;
    };

    MARATHON_ASSERT_OFFSETOF(WorldIntersectionCSM, m_pCamera, 0x18);
    MARATHON_ASSERT_SIZEOF(WorldIntersectionCSM, 0x1C);
}
