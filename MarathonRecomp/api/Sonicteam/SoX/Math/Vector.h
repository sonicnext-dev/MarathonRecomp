#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Math
{
    class alignas(16) Vector
    {
    public:
        be<float> X{};
        be<float> Y{};
        be<float> Z{};
        be<float> W{};

        Vector() = default;

        Vector(float in_x, float in_y, float in_z, float in_w = 1.0f)
        {
            X = in_x;
            Y = in_y;
            Z = in_z;
            W = in_w;
        }

        static Vector Zero()
        {
            return Vector(0, 0, 0, 0);
        }

        bool IsZero() const
        {
            return X == 0 && Y == 0 && Z == 0 && W == 0;
        }

        float Distance(const Vector& in_rOther) const
        {
            const auto dx = X - in_rOther.X;
            const auto dy = Y - in_rOther.Y;
            const auto dz = Z - in_rOther.Z;

            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }

        float Dot(const Vector& in_rOther) const
        {
            return (X * in_rOther.X) + (Y * in_rOther.Y) + (Z * in_rOther.Z);
        }

        Vector operator+(float in_addend) const
        {
            return { X + in_addend, Y + in_addend, Z + in_addend, W + in_addend };
        }

        Vector operator+(const Vector& in_rOther) const
        {
            return { X + in_rOther.X, Y + in_rOther.Y, Z + in_rOther.Z, W + in_rOther.W };
        }

        Vector& operator+=(float in_addend)
        {
            X = X + in_addend;
            Y = Y + in_addend;
            Z = Z + in_addend;
            W = W + in_addend;

            return *this;
        }

        Vector& operator+=(const Vector& in_rOther)
        {
            X = X + in_rOther.X;
            Y = Y + in_rOther.Y;
            Z = Z + in_rOther.Z;
            W = W + in_rOther.W;

            return *this;
        }

        Vector operator-(float in_subtrahend) const
        {
            return { X - in_subtrahend, Y - in_subtrahend, Z - in_subtrahend, W - in_subtrahend };
        }

        Vector operator-(const Vector& in_rOther) const
        {
            return { X - in_rOther.X, Y - in_rOther.Y, Z - in_rOther.Z, W - in_rOther.W };
        }

        Vector& operator-=(float in_subtrahend)
        {
            X = X - in_subtrahend;
            Y = Y - in_subtrahend;
            Z = Z - in_subtrahend;
            W = W - in_subtrahend;

            return *this;
        }

        Vector& operator-=(const Vector& in_rOther)
        {
            X = X - in_rOther.X;
            Y = Y - in_rOther.Y;
            Z = Z - in_rOther.Z;
            W = W - in_rOther.W;

            return *this;
        }

        Vector operator*(float in_multiplier) const
        {
            return { X * in_multiplier, Y * in_multiplier, Z * in_multiplier, W * in_multiplier };
        }

        Vector operator*(const Vector& in_rOther) const
        {
            return { X * in_rOther.X, Y * in_rOther.Y, Z * in_rOther.Z, W * in_rOther.W };
        }

        Vector& operator*=(float in_multiplier)
        {
            X = X * in_multiplier;
            Y = Y * in_multiplier;
            Z = Z * in_multiplier;
            W = W * in_multiplier;

            return *this;
        }

        Vector& operator*=(const Vector& in_rOther)
        {
            X = X * in_rOther.X;
            Y = Y * in_rOther.Y;
            Z = Z * in_rOther.Z;
            W = W * in_rOther.W;

            return *this;
        }
    };

    MARATHON_ASSERT_OFFSETOF(Vector, X, 0x00);
    MARATHON_ASSERT_OFFSETOF(Vector, Y, 0x04);
    MARATHON_ASSERT_OFFSETOF(Vector, Z, 0x08);
    MARATHON_ASSERT_OFFSETOF(Vector, W, 0x0C);
    MARATHON_ASSERT_SIZEOF(Vector, 0x10);
}
