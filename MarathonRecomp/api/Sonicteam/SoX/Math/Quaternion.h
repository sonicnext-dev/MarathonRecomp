#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/__vector.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <algorithm>

namespace Sonicteam::SoX::Math
{

    class alignas(16) Quaternion : public __vector
    {
    public:

        Quaternion() :__vector(0, 0, 0, 1) {}
        Quaternion(float x, float y = 0, float z = 0, float w = 0) :__vector(x, y, z, w) {}

        Quaternion(__vector v) :__vector(v.X, v.Y, v.Z, v.W) {};

        Quaternion(DirectX::XMVECTOR vec) : __vector(vec) {}
        Quaternion(DirectX::XMFLOAT4 vec) : __vector(vec) {}

        Quaternion operator*(float scalar) const
        {
            return { X * scalar, Y * scalar, Z * scalar, W * scalar };
        }

        Quaternion operator*(const Quaternion& quat) const
        {
            return DirectX::XMQuaternionMultiply(*this, quat);
        }

        Vector operator*(const Vector& vector) const
        {
            return DirectX::XMVector3Rotate(vector, *this);
        }

        static const Quaternion Identity()
        {
            return { 0 , 0, 0, 1 };
        }

        Vector GetForwardVector() const
        {
            return DirectX::XMVector3Rotate(Vector::Forward(), *this);
        }

        Vector GetUpVector() const
        {
            return DirectX::XMVector3Rotate(Vector::Up(), *this);
        }

        Vector GetRightVector() const
        {
            return DirectX::XMVector3Rotate(Vector::Right(), *this);
        }

        static Quaternion FromVector(const Vector& direction, const Vector& up = Vector::Up())
        {
            Vector normalizedDir = direction.Normalize3();
            if (DirectX::XMVectorGetX(DirectX::XMVector3Length(normalizedDir)) < 0.001f)
                return Identity();

            Vector zaxis = normalizedDir;
            Vector xaxis = Vector::Cross(up, zaxis).Normalize3();
            Vector yaxis = Vector::Cross(zaxis, xaxis);

            DirectX::XMMATRIX mat = DirectX::XMMatrixSet(
                xaxis.X, xaxis.Y, xaxis.Z, 0.0f,
                yaxis.X, yaxis.Y, yaxis.Z, 0.0f,
                zaxis.X, zaxis.Y, zaxis.Z, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            );

            return DirectX::XMQuaternionRotationMatrix(mat);
        }

        //sub_821E8778 --done--
        static Quaternion CreateFromVectors(Vector forward, Vector target, Vector up)
        {
            auto cross1 = Vector::Cross(up, forward).Normalize3();
            auto cross2 = Vector::Cross(up, target).Normalize3();

            auto rotationAxis = Vector::Cross(cross1, cross2);
            auto [normalizedAxis, axisLengthZero] = rotationAxis.NormalizeEX();

            float dot = Vector::Dot(cross2, cross1).X.get();
            dot = std::clamp(dot, -1.0f, 1.0f);

            float sinHalf = std::sqrt((1.0f - dot) * 0.5f);
            float cosHalf = std::sqrt((1.0f + dot) * 0.5f);

            if (sinHalf != 0.0f && axisLengthZero == 0.0f)
            {
                normalizedAxis = up.Normalize3();
            }

            return Quaternion(
                normalizedAxis.X * sinHalf,
                normalizedAxis.Y * sinHalf,
                normalizedAxis.Z * sinHalf,
                cosHalf
            );
        }

        static Quaternion Slerp(Quaternion q1, Quaternion q2, float lerp)
        {
            return DirectX::XMQuaternionSlerp(q1, q2, lerp);;
        }

        Quaternion Slerp(Quaternion q2, float lerp)
        {
            return Slerp(*this, q2, lerp);
        }

        static Quaternion Inverse(Quaternion q1)
        {
            return DirectX::XMQuaternionInverse(q1);;
        }

        Quaternion Inverse()
        {
            return Inverse(*this);
        }
    };

    inline Quaternion __vector::AsQuaternion() const
    {
        return Quaternion(X, Y, Z, W);
    }

    inline Vector __vector::AsVector() const
    {
        return Vector(X, Y, Z, W);
    }

    inline Quaternion Vector::ToQuaternion(float angle) const
    {
        Vector axis = this->Normalize3();
        return DirectX::XMQuaternionRotationAxis(axis, angle);
    }

    inline Quaternion Vector::RotationTo(const Vector& target) const
    {
        Vector from = this->Normalize3();
        Vector to = target.Normalize3();

        float dot = DirectX::XMVectorGetX(Vector::Dot(from, to));
        dot = std::clamp(dot, -1.0f, 1.0f);

        if (dot > 0.9999f) return Quaternion::Identity();

        Vector axis = Vector::Cross(from, to).Normalize3();
        float angle = std::acos(dot);
        return axis.ToQuaternion(angle);
    }
}
