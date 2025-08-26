#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Math
{
    class alignas(16) Quaternion
    {
    public:
        be<float> X;
        be<float> Y;
        be<float> Z;
        be<float> W;
    };
}
