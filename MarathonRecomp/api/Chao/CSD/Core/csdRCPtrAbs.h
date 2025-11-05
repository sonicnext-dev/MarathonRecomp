#pragma once

#include <Marathon.inl>

namespace Chao::CSD
{
    class RCPtrAbs
    {
    public:
        class RCObject;

        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpCreateRCObject;
        };

        xpointer<Vftable> m_pVftable;
        xpointer<RCObject> m_pObject;
    };

    MARATHON_ASSERT_OFFSETOF(RCPtrAbs, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(RCPtrAbs, m_pObject, 0x04);
}
