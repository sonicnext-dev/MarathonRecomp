#pragma once

#include <Sonicteam/SoX/Physics/ShapeCast.h>

namespace Sonicteam::SoX::Physics::Havok
{
    class ShapeCastHavok : public ShapeCast
    {
    public:
        MARATHON_INSERT_PADDING(0x04);
    };

    MARATHON_ASSERT_SIZEOF(ShapeCastHavok, 0x10);
}
