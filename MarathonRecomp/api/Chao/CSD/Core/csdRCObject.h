#pragma once

#include <Chao/CSD/Core/csdRCPtrAbs.h>

namespace Chao::CSD
{
    class RCPtrAbs::RCObject
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpFree;
        };

        xpointer<Vftable> m_pVftable;
        xpointer<void> m_pMemory;
        be<uint32_t> m_ReferenceCount;
        xpointer<void> m_Field0C;
        be<uint32_t> m_Field10;
    };

    MARATHON_ASSERT_OFFSETOF(RCPtrAbs::RCObject, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(RCPtrAbs::RCObject, m_pMemory, 0x04);
    MARATHON_ASSERT_OFFSETOF(RCPtrAbs::RCObject, m_ReferenceCount, 0x08);
    MARATHON_ASSERT_OFFSETOF(RCPtrAbs::RCObject, m_Field0C, 0x0C);
    MARATHON_ASSERT_OFFSETOF(RCPtrAbs::RCObject, m_Field10, 0x10);
}
