#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Math
{
    class Vector2
    {
    public:
        be<float> X;
        be<float> Y;
    };

    class Vector3
    {
    public:
        be<float> X;
        be<float> Y;
        be<float> Z;
    };

    class alignas(16) Vector
    {
    public:
        be<float> X;
        be<float> Y;
        be<float> Z;
        be<float> W;

        Vector operator+(float addend)
        {
            return { X + addend, Y + addend, Z + addend, W + addend };
        }

        float DistanceTo(const Vector& other) const
        {
            float dx = static_cast<float>(X) - static_cast<float>(other.X);
            float dy = static_cast<float>(Y) - static_cast<float>(other.Y);
            float dz = static_cast<float>(Z) - static_cast<float>(other.Z);
            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }
    };
}
