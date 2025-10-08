#pragma once

#include "Marathon.inl"
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Math/Quaternion.h>

namespace Sonicteam::SoX::Math
{
    class Matrix4x4
    {
    public:
        union
        {
            be<float> M[4][4];
            Vector V[4];
        };
      

        Matrix4x4() {}

        Matrix4x4(Matrix4x4& other)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                    M[i][j] = other.M[i][j];
            }
        }

        Matrix4x4(DirectX::XMMATRIX& other)
        {
            for (int i = 0; i < 4; i++)
            {
                V[i] = other.r[i];
            }
        }

        Matrix4x4(const DirectX::XMMATRIX& other)
        {
            for (int i = 0; i < 4; i++)
            {
                V[i] = other.r[i];
            }
        }

        operator DirectX::XMMATRIX() const
        {
            return DirectX::XMMATRIX(V[0], V[1], V[2], V[3]);
        }

        Matrix4x4 Multiply(const Matrix4x4& other) const
        {
            Matrix4x4 result{};

            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    result.M[i][j] = 0.0f;

                    for (int k = 0; k < 4; k++)
                        result.M[i][j] = result.M[i][j] + (M[i][k] * other.M[k][j]);
                }
            }

            return result;
        }

        Matrix4x4 operator*(Matrix4x4 other)
        {
            return DirectX::XMMatrixMultiply(*this, other);
        }

        static Matrix4x4 RotationQuaternion(const Quaternion& rotation)
        {
            return DirectX::XMMatrixRotationQuaternion(rotation);
        }

        static Matrix4x4 TranslationFromVector(const Vector& translation)
        {
            return DirectX::XMMatrixTranslationFromVector(translation);
        }

        static Matrix4x4 LookToRH(const Vector& eye, const Vector& focus, const Vector& up)
        {
            return DirectX::XMMatrixLookAtRH(eye, focus, up);
        }
    };
}
