#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Math
{
    class Vector2
    {
    public:
        be<float> X;
        be<float> Y;

        bool IsZero() const
        {
            return X == 0 && Y == 0;
        }

        bool operator==(const Vector2& in_rOther) const
        {
            return X == in_rOther.X && Y == in_rOther.Y;
        }
    };

    MARATHON_ASSERT_OFFSETOF(Vector2, X, 0x00);
    MARATHON_ASSERT_OFFSETOF(Vector2, Y, 0x04);
    MARATHON_ASSERT_SIZEOF(Vector2, 0x08);
}
