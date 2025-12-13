#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/AckAdminTag.h>

namespace Sonicteam::Spanverse
{
    class AckTrunkObject;

    class AckAdminTrunkTag : public AckAdminTag
    {
    public:
        xpointer<AckTrunkObject> m_pObject;
    };

    MARATHON_ASSERT_OFFSETOF(AckAdminTrunkTag, m_pObject,0xC);
    MARATHON_ASSERT_SIZEOF(AckAdminTrunkTag, 0x10);
}
