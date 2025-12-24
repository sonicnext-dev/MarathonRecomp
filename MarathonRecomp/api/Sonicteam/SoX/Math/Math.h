#pragma once

#include "Marathon.inl"
#include <Sonicteam/SoX/Math/Matrix.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Math/Quaternion.h>
#include <DirectXMath.h>

namespace Sonicteam::SoX::Math
{
    inline Matrix4x4 MatrixFromQuaternion(const Quaternion& quat)
    {
        Matrix4x4 result;

        float xx = quat.X * quat.X;
        float yy = quat.Y * quat.Y;
        float zz = quat.Z * quat.Z;
        float xy = quat.X * quat.Y;
        float xz = quat.X * quat.Z;
        float yz = quat.Y * quat.Z;
        float wx = quat.W * quat.X;
        float wy = quat.W * quat.Y;
        float wz = quat.W * quat.Z;

        result.M[0][0] = 1.0f - 2.0f * (yy + zz);
        result.M[0][1] = 2.0f * (xy + wz);
        result.M[0][2] = 2.0f * (xz - wy);
        result.M[0][3] = 0.0f;

        result.M[1][0] = 2.0f * (xy - wz);
        result.M[1][1] = 1.0f - 2.0f * (xx + zz);
        result.M[1][2] = 2.0f * (yz + wx);
        result.M[1][3] = 0.0f;

        result.M[2][0] = 2.0f * (xz + wy);
        result.M[2][1] = 2.0f * (yz - wx);
        result.M[2][2] = 1.0f - 2.0f * (xx + yy);
        result.M[2][3] = 0.0f;

        result.M[3][0] = 0.0f;
        result.M[3][1] = 0.0f;
        result.M[3][2] = 0.0f;
        result.M[3][3] = 1.0f;

        return result;
    }

    inline Vector TransformCoord(const Vector& vector, const Matrix4x4& matrix)
    {
        Vector result;

        result.X = vector.X * matrix.M[0][0] + vector.Y * matrix.M[1][0] + vector.Z * matrix.M[2][0] + matrix.M[3][0];
        result.Y = vector.X * matrix.M[0][1] + vector.Y * matrix.M[1][1] + vector.Z * matrix.M[2][1] + matrix.M[3][1];
        result.Z = vector.X * matrix.M[0][2] + vector.Y * matrix.M[1][2] + vector.Z * matrix.M[2][2] + matrix.M[3][2];

        float w = vector.X * matrix.M[0][3] + vector.Y * matrix.M[1][3] + vector.Z * matrix.M[2][3] + matrix.M[3][3];

        if (w != 0.0f && w != 1.0f)
        {
            result.X = result.X.get() / w;
            result.Y = result.Y.get() / w;
            result.Z = result.Z.get() / w;

        }

        return result;
    }

    //  Calculates a normalized surface vector based on gravity direction. (analog to 821E8578)
    static Quaternion CreateRotationFromUpToDirection(DirectX::XMVECTOR inputDirection)
    {
        const DirectX::XMVECTOR upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

        const float epsilon = DirectX::XMVectorGetX(DirectX::XMVectorSplatEpsilon());

        // 1. Load and normalize input vector
        DirectX::XMVECTOR direction = inputDirection;
        DirectX::XMVECTOR normDirection = DirectX::XMVector3Normalize(direction);

        // 2. Check for zero-length input
        if (DirectX::XMVector3NearEqual(normDirection, DirectX::XMVectorZero(), DirectX::XMVectorReplicate(epsilon)))
        {
            return DirectX::XMQuaternionIdentity();
        }

        // 3. Calculate orthogonal basis
        DirectX::XMVECTOR cross = DirectX::XMVector3Cross(upVector, normDirection);
        DirectX::XMVECTOR normCross = DirectX::XMVector3Normalize(cross);

        // 4. Check for colinear vectors
        if (DirectX::XMVector3NearEqual(normCross, DirectX::XMVectorZero(), DirectX::XMVectorReplicate(epsilon)))
        {
            return DirectX::XMQuaternionIdentity();
        }

        // 5. Calculate angle components
        DirectX::XMVECTOR dot = DirectX::XMVector3Dot(normDirection, upVector);
        float cosAngle = DirectX::XMVectorGetX(DirectX::XMVectorClamp(dot,
            DirectX::XMVectorReplicate(-1.0f),
            DirectX::XMVectorReplicate(1.0f)));

        // 6. Create rotation quaternion
        DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationAxis(normCross, DirectX::XMScalarACos(cosAngle));

        // Convert XMVECTOR to XMFLOAT4 for return
        DirectX::XMFLOAT4 result;
        DirectX::XMStoreFloat4(&result, quat);
        return result;
    }

    static Quaternion QuaternionMultiply(const DirectX::XMVECTOR q1, const DirectX::XMVECTOR q2)
    {
        return DirectX::XMQuaternionMultiply(q1, q2);
    }

    static Quaternion CreateRotationFromForwardUp(const DirectX::XMVECTOR& forward, const DirectX::XMVECTOR& up)
    {
        DirectX::XMVECTOR rotationQuat = DirectX::XMQuaternionRotationMatrix(
            DirectX::XMMatrixLookToLH(DirectX::XMVectorZero(), forward, up)
        );

        return rotationQuat;
    }

    static Vector QuaternionToVector(Quaternion quat)
    {
        return *(Vector*)&quat;
    }
}
