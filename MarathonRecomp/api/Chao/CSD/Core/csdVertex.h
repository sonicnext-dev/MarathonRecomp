#pragma once

#include <Marathon.inl>
#include <Chao/CSD/Core/csdColor.h>

namespace Chao::CSD
{
    struct Vertex
    {
        be<float> X;
        be<float> Y;
        Color Color;
        be<float> U;
        be<float> V;
    };

    MARATHON_ASSERT_OFFSETOF(Vertex, X, 0x00);
    MARATHON_ASSERT_OFFSETOF(Vertex, Y, 0x04);
    MARATHON_ASSERT_OFFSETOF(Vertex, Color, 0x08);
    MARATHON_ASSERT_OFFSETOF(Vertex, U, 0x0C);
    MARATHON_ASSERT_OFFSETOF(Vertex, V, 0x10);
    MARATHON_ASSERT_SIZEOF(Vertex, 0x14);
}
