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

    MARATHON_ASSERT_OFFSETOF(IResourceMgr, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IResourceMgr, 0x10);
}
