#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class TextCard
    {
    public:
        boost::shared_ptr<uint8_t> m_spResource;
        xpointer<const uint16_t> m_pText;
        xpointer<const char> m_pVariables;
    };
}
