#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Graphics
{
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

    MARATHON_ASSERT_OFFSETOF(Vertex, X, 0x00);
    MARATHON_ASSERT_OFFSETOF(Vertex, Y, 0x04);
    MARATHON_ASSERT_OFFSETOF(Vertex, Colour, 0x18);
    MARATHON_ASSERT_OFFSETOF(Vertex, U, 0x1C);
    MARATHON_ASSERT_OFFSETOF(Vertex, V, 0x20);
    MARATHON_ASSERT_SIZEOF(Vertex, 0x3C);
}
