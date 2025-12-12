#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Graphics
{
    class Technique : public RefCountObject
    {
    public:
        xpointer<void> m_pShader;
    };

    MARATHON_ASSERT_OFFSETOF(Technique, m_pShader, 0x08);
    MARATHON_ASSERT_SIZEOF(Technique, 0x0C);
}