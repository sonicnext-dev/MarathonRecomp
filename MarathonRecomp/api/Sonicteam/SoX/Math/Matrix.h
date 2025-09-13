#pragma once

#include "Marathon.inl"

namespace Sonicteam::SoX::Math
{
    class Matrix4x4
    {
    public:
        be<float> M[4][4];

        Matrix4x4() {}

        Matrix4x4(Matrix4x4& other)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                    M[i][j] = other.M[i][j];
            }
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
            return Multiply(other);
        }
    };
}
