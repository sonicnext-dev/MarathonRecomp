#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Graphics
{
    class Technique : public RefCountObject
    {
    public:
        xpointer<void> m_pShader;
    };
}
