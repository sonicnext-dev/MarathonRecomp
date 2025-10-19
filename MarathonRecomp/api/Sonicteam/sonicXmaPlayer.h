#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class sonicXmaPlayer : public SoX::RefCountObject
    {
    public:
        MARATHON_INSERT_PADDING(0xD8);
        bool m_IsPaused;
    };
}
