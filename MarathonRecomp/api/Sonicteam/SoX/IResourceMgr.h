#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX
{
    class IResourceMgr
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpMakeResource;
            MARATHON_INSERT_PADDING(4);
        };

        xpointer<Vftable> m_pVftable;
        MARATHON_INSERT_PADDING(0x0C);
    };
}
