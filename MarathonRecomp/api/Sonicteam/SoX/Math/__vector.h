#pragma once

#include <Marathon.inl>
#include <DirectXMath.h>
#include <string>
#include <fmt/format.h>

namespace Sonicteam::SoX::Math
{
    // Forward declarations
    class Vector;
    class Quaternion;

    struct alignas(16) __vector
    {
    public:
        be<float> X;
        be<float> Y;
        be<float> Z;
        be<float> W;

        __vector()
            : X(0)
            , Y(0)
            , Z(0)
            , W(0)
        {
        }

        __vector(float x, float y, float z, float w)
            : X(x)
            , Y(y)
            , Z(z)
            , W(w)
        {
        }

        __vector(float x, float y, float z)
            : X(x)
            , Y(y)
            , Z(z)
            , W(0)
        {
        }

        __vector(const DirectX::XMFLOAT4& vec)
            : X(vec.x)
            , Y(vec.y)
            , Z(vec.z)
            , W(vec.w)
        {
        }

        __vector(const DirectX::XMFLOAT3& vec)
            : X(vec.x)
            , Y(vec.y)
            , Z(vec.z)
            , W(0)
        {
        }

        __vector(const DirectX::XMVECTOR& vec)
        {
            DirectX::XMFLOAT4 temp;
            DirectX::XMStoreFloat4(&temp, vec);
            X = temp.x;
            Y = temp.y;
            Z = temp.z;
            W = temp.w;
        }

        // Conversion operators
        operator DirectX::XMFLOAT4() const
        {
            return DirectX::XMFLOAT4(
                static_cast<float>(X),
                static_cast<float>(Y),
                static_cast<float>(Z),
                static_cast<float>(W)
            );
        }

        operator DirectX::XMFLOAT3() const
        {
            return DirectX::XMFLOAT3(
                static_cast<float>(X),
                static_cast<float>(Y),
                static_cast<float>(Z)
            );
        }

        operator DirectX::XMVECTOR() const
        {
            return DirectX::XMVectorSet(
                this->X,
                this->Y,
                this->Z,
                this->W
            );
        }

        __vector operator*(float scalar) const
        {
            return { this->X * scalar, this->Y * scalar, this->Z * scalar, this->W * scalar };
        }

        __vector operator+(__vector v) const
        {
            return { this->X + v.X, this->Y + v.Y, this->Z + v.Z, this->W + v.W };
        }

        __vector operator-(__vector v) const
        {
            return { this->X - v.X, this->Y - v.Y, this->Z - v.Z, this->W - v.W };
        }

        // Universal conversion methods (declarations only)
        Quaternion AsQuaternion() const;
        Vector AsVector() const;

        std::string tostring() const
        {
            return fmt::format("({}, {}, {}, {})", X.get(), Y.get(), Z.get(), W.get());
        }
    };
}
