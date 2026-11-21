#pragma once

#include <Marathon.inl>
#include <Kaim/CObject.h>

namespace Kaim
{
    class CBaseService : public CObject
    {
    public:
        struct Vftable : public CObject::Vftable
        {
            be<uint32_t> fpInit;
            be<uint32_t> fpInitParam;
            be<uint32_t> fpReInit;
        };
    };

    MARATHON_ASSERT_SIZEOF(CBaseService, 0x04);
}
