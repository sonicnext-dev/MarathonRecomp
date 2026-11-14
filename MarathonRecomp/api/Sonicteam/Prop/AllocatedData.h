#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::Prop
{
    class AllocatedData : public SoX::RefCountObject
    {
    public:
        xpointer<uint8_t> m_pResource;
        be<uint32_t> m_ResourceLength;
    };

    MARATHON_ASSERT_OFFSETOF(AllocatedData, m_pResource, 0x08);
    MARATHON_ASSERT_OFFSETOF(AllocatedData, m_ResourceLength, 0x0C);
    MARATHON_ASSERT_SIZEOF(AllocatedData, 0x10);
}
