#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Math
{
    class alignas(16) Quaternion
    {
    public:
        be<float> X;
        be<float> Y;
        be<float> Z;
        be<float> W;
    };

    MARATHON_ASSERT_OFFSETOF(Quaternion, X, 0x00);
    MARATHON_ASSERT_OFFSETOF(Quaternion, Y, 0x04);
    MARATHON_ASSERT_OFFSETOF(Quaternion, Z, 0x08);
    MARATHON_ASSERT_OFFSETOF(Quaternion, W, 0x0C);
    MARATHON_ASSERT_SIZEOF(Quaternion, 0x10);
}
