#pragma once

#include <CSD/Manager/csdmRCPtrAbs.h>

namespace Chao::CSD
{
    class RCPtrAbs::RCObject
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpDeallocate;
        };

        xpointer<Vftable> m_pVftable;
        xpointer<void> m_pMemory;
        be<uint32_t> m_RefCount;
        xpointer<void> m_pDealloctor;
        be<uint32_t> m_eDealloctor;
    };
}
