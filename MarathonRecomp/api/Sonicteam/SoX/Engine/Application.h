#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Engine
{
    class Application
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(8);
    };
}
