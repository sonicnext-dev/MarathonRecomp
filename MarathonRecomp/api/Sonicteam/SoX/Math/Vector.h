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

    class Vector
    {
    public:
        be<float> X;
        be<float> Y;
        be<float> Z;
        be<float> W;
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