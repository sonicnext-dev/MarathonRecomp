#pragma once

#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::SoX::Math
{
    struct Distance
    {
        Vector Min{};
        Vector Max{};

        Distance() = default;

        Distance(const Vector& in_rMin, const Vector& in_rMax)
            : Min(in_rMin), Max(in_rMax) {}
    };
}
