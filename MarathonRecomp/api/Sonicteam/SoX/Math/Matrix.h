#pragma once

#include "Marathon.inl"

namespace Sonicteam::SoX::Math
{
    class Matrix4x4
    {
    public:
        be<float> M00;
        be<float> M01;
        be<float> M02;
        be<float> M03;
        be<float> M10;
        be<float> M11;
        be<float> M12;
        be<float> M13;
        be<float> M20;
        be<float> M21;
        be<float> M22;
        be<float> M23;
        be<float> M30;
        be<float> M31;
        be<float> M32;
        be<float> M33;
    };
}
