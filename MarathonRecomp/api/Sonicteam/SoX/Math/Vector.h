#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/__vector.h>
#include <DirectXMath.h>

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

    class alignas(16) Vector : public __vector
    {
    public:
        Vector() :__vector()
        {
        }

        Vector(__vector v) :__vector(v.X, v.Y, v.Z, v.W)
        {
        }

        Vector(float x, float y = 0.0f, float z = 0.0f, float w = 0.0f) :__vector(x, y, z, w)
        {
        }

        Vector(DirectX::XMVECTOR vec) : __vector(vec)
        {
        }

        Vector(DirectX::XMFLOAT4 vec) : __vector(vec)
        {
        }

        Vector operator+(float scalar) const
        {
            return { X + scalar, Y + scalar, Z + scalar, W + scalar };
        }

        Vector operator+(Vector v2) const
        {
            return { X + v2.X, Y + v2.Y, Z + v2.Z, W + v2.W };
        }

        Vector operator-(Vector v2) const
        {
            return { X - v2.X, Y - v2.Y, Z - v2.Z, W - v2.W };
        }

        Vector operator*(float scalar) const
        {
            return { X * scalar, Y * scalar, Z * scalar, W * scalar };
        }

        Vector operator/(float scalar) const
        {
            return { X / scalar, Y / scalar, Z / scalar, W / scalar };
        }

        Vector operator*(const Vector& scalar) const
        {
            return { X * scalar.X, Y * scalar.Y, Z * scalar.Z, 1.0 };
        }

        float DistanceTo(const Vector& other) const
        {
            float dx = static_cast<float>(X) - static_cast<float>(other.X);
            float dy = static_cast<float>(Y) - static_cast<float>(other.Y);
            float dz = static_cast<float>(Z) - static_cast<float>(other.Z);
            return sqrtf(dx * dx + dy * dy + dz * dz);
        }

        static Vector Cross(const Vector& first, const Vector& second)
        {
            return {
                first.Y * second.Z - first.Z * second.Y,
                first.Z * second.X - first.X * second.Z,
                first.X * second.Y - first.Y * second.X
            };
        }

        static Vector Dot(const Vector& first, const Vector& second)
        {
            return DirectX::XMVector3Dot(first, second);
        }

        static Vector Slerp(const Vector& first, const Vector& second, float lerp = 1.0f)
        {
            return DirectX::XMQuaternionSlerp(first, second, lerp);
        }

        static Vector Length3(const Vector& first)
        {
            return DirectX::XMVector3Length(first);
        }

        static Vector Normalize(const Vector& first)
        {
            return DirectX::XMVector3Normalize(first);
        }

        static Vector Normalize3(const Vector& first)
        {
            return DirectX::XMVector3Normalize(first);
        }

        // Analog to sub_825D22A0
        static std::pair<Vector, float> NormalizeEX(const Vector& first)
        {
            float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(first));
            return { DirectX::XMVector3Normalize(first), length };
        }

        Vector Cross(const Vector& other) const
        {
            return Cross(*this, other);
        }

        Vector Dot(const Vector& other) const
        {
            return Dot(*this, other);
        }

        Vector Slerp(const Vector& other, float lerp = 1.0f) const
        {
            return Slerp(*this, other, lerp);
        }

        Vector Normalize3() const
        {
            return Normalize3(*this);
        }

        std::pair<Vector, float> NormalizeEX() const
        {
            return NormalizeEX(*this);
        }

        Vector Length3() const
        {
            return Length3(*this);
        }

        Vector InvertNormal() const
        {
            return { -X, -Y, -Z, 1.0f };
        }

        Quaternion ToQuaternion(float angle) const;
        Quaternion RotationTo(const Vector& target) const;

        static const Vector Forward()
        {
            return { 0.0f, 0.0f, 1.0f, 1.0f };
        }

        static const Vector Up()
        {
            return { 0.0f, 1.0f, 0.0f, 1.0f };
        }

        static const Vector Zero()
        {
            return { 0.0f, 0.0f, 0.0f, 1.0f };
        }

        static const Vector Right()
        {
            return { 1.0f, 0.0f, 0.0f, 1.0f };
        }

        static const Vector Left()
        {
            return { -1.0f, 0.0f, 0.0f, 1.0f };
        }
    };
}
