#pragma once

#include <cmath>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::SoX::Math
{
    inline Vector Hermite(const Vector& in_rStart, const Vector& in_rEnd, float& io_time, const float in_speed, const float in_deltaTime)
    {
        io_time = std::min(io_time + (in_deltaTime * in_speed), 1.0f);

        const auto progress = io_time * io_time * (3.0f - 2.0f * io_time);

        return in_rStart + ((in_rEnd - in_rStart) * progress);
    }

    inline float WrapAngle(const float in_radians)
    {
        const auto sign = in_radians > 0.0f ? 1.0f : -1.0f;

        return std::fmodf(std::abs(in_radians), 6.28319f) * sign;
    }
}
