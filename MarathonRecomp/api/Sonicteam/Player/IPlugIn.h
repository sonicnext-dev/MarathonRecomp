#pragma once

#include <Marathon.inl>
#include <stdx/string.h>

namespace Sonicteam::Player
{
    class IPlugIn
    {
    public:
        xpointer<void> m_pVftable;
        stdx::string m_Name;
    };
}
