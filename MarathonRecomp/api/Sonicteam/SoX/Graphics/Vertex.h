#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Color.h>

namespace Sonicteam::SoX::Graphics
{
    struct Vertex
    {
        be<float> X;
        be<float> Y;
        MARATHON_INSERT_PADDING(0x10);
        ARGB8888 Color;
        be<float> U;
        be<float> V;
        MARATHON_INSERT_PADDING(0x18);
    };

    MARATHON_ASSERT_OFFSETOF(Vertex, X, 0x00);
    MARATHON_ASSERT_OFFSETOF(Vertex, Y, 0x04);
    MARATHON_ASSERT_OFFSETOF(Vertex, Color, 0x18);
    MARATHON_ASSERT_OFFSETOF(Vertex, U, 0x1C);
    MARATHON_ASSERT_OFFSETOF(Vertex, V, 0x20);
    MARATHON_ASSERT_SIZEOF(Vertex, 0x3C);
}
