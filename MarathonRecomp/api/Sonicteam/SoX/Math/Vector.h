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

        Vector() = default;

        Vector(float in_x, float in_y, float in_z, float in_w = 1.0f)
        {
            X = in_x;
            Y = in_y;
            Z = in_z;
            W = in_w;
        }

        float DistanceTo(const Vector& in_rOther) const
        {
            float dx = static_cast<float>(X) - static_cast<float>(in_rOther.X);
            float dy = static_cast<float>(Y) - static_cast<float>(in_rOther.Y);
            float dz = static_cast<float>(Z) - static_cast<float>(in_rOther.Z);

            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }

        bool IsZero() const
        {
            return X == 0 && Y == 0 && Z == 0 && W == 0;
        }

        Vector operator+(float in_addend) const
        {
            return { X + in_addend, Y + in_addend, Z + in_addend, W + in_addend };
        }

        Vector operator+(const Vector& in_rOther) const
        {
            return { X + in_rOther.X, Y + in_rOther.Y, Z + in_rOther.Z, W + in_rOther.W };
        }

        Vector operator-(float in_subtrahend) const
        {
            return { X - in_subtrahend, Y - in_subtrahend, Z - in_subtrahend, W - in_subtrahend };
        }

        Vector operator-(const Vector& in_rOther) const
        {
            return { X - in_rOther.X, Y - in_rOther.Y, Z - in_rOther.Z, W - in_rOther.W };
        }

        Vector operator*(float in_multiplier) const
        {
            return { X * in_multiplier, Y * in_multiplier, Z * in_multiplier, W * in_multiplier };
        }

        Vector operator*(const Vector& in_rOther) const
        {
            return { X * in_rOther.X, Y * in_rOther.Y, Z * in_rOther.Z, W * in_rOther.W };
        }
    };
}
