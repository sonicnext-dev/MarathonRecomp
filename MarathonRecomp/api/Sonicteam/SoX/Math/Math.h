#pragma once

#include <cmath>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::SoX::Math
{
    // Constants from "player/common.lub".
    static constexpr double Pi = 3.141592653589793;
    static constexpr double Meter = 100.0;
    static constexpr double Kilometer = 1000.0 * Meter;
    static constexpr double Kilogram = 1000.0;
    static constexpr double Second = 1.0;
    static constexpr double Gravity = 9.8 * Meter / (Second * Second);
    static constexpr double Point = 1.0;
    static constexpr double Degrees = Pi / 180.0;
    static constexpr double Radians = 180.0 / Pi;
    static constexpr double Newtons = Meter / (Second * Second);

    inline Vector Hermite(const Vector& in_rStart, const Vector& in_rEnd, float& io_time, const float in_speed, const float in_deltaTime)
    {
        io_time = std::min(io_time + (in_deltaTime * in_speed), 1.0f);

        const auto progress = io_time * io_time * (3.0f - 2.0f * io_time);

        return in_rStart + ((in_rEnd - in_rStart) * progress);
    }

    inline constexpr double ToDegrees(double in_radians)
    {
        return in_radians * Radians;
    }

    inline constexpr double ToRadians(double in_degrees)
    {
        return in_degrees * Degrees;
    }

    inline constexpr float WrapAngle(const float in_radians)
    {
        const auto sign = in_radians > 0.0f ? 1.0f : -1.0f;

        return std::fmodf(std::abs(in_radians), 6.28319f) * sign;
    }
}
