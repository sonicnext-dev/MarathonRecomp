#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX
{
    class IResourceMgr
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x0C);
    };
}
