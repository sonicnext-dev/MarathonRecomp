#pragma once

#include <cmath>

namespace Sonicteam::SoX::Math
{
    inline float WrapAngle(const float in_radians)
    {
        const auto sign = in_radians > 0.0f ? 1.0f : -1.0f;

        return std::fmodf(std::abs(in_radians), 6.28319f) * sign;
    }
}
