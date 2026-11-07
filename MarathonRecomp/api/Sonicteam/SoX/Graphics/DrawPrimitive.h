#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector2.h>

namespace Sonicteam::SoX::Graphics
{
    struct DrawPrimitive
    {
        be<float> Field00;
        be<float> Field04;
        be<float> Field08;
        be<float> Field0C;
        be<float> Field10;
        be<float> Field14;
        be<uint32_t> Field18;
        Math::Vector2 Field1C[4];
    };

    MARATHON_ASSERT_OFFSETOF(DrawPrimitive, Field00, 0x00);
    MARATHON_ASSERT_OFFSETOF(DrawPrimitive, Field04, 0x04);
    MARATHON_ASSERT_OFFSETOF(DrawPrimitive, Field08, 0x08);
    MARATHON_ASSERT_OFFSETOF(DrawPrimitive, Field0C, 0x0C);
    MARATHON_ASSERT_OFFSETOF(DrawPrimitive, Field10, 0x10);
    MARATHON_ASSERT_OFFSETOF(DrawPrimitive, Field14, 0x14);
    MARATHON_ASSERT_OFFSETOF(DrawPrimitive, Field18, 0x18);
    MARATHON_ASSERT_OFFSETOF(DrawPrimitive, Field1C, 0x1C);
    MARATHON_ASSERT_SIZEOF(DrawPrimitive, 0x3C);
}
