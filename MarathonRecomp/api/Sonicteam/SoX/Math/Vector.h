#pragma once

#include "Marathon.inl"

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

    class Vector4
    {
    public:
        be<float> X;
        be<float> Y;
        be<float> Z;
        be<float> W;
    };
}
