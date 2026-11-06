#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Math
{
    class Vector3
    {
    public:
        be<float> X;
        be<float> Y;
        be<float> Z;
    };

    MARATHON_ASSERT_OFFSETOF(Vector3, X, 0x00);
    MARATHON_ASSERT_OFFSETOF(Vector3, Y, 0x04);
    MARATHON_ASSERT_OFFSETOF(Vector3, Z, 0x08);
    MARATHON_ASSERT_SIZEOF(Vector3, 0x0C);
}
