#pragma once

#include <Marathon.inl>
#include <Kaim/CBaseService.h>

namespace Kaim
{
    class CBrain;

    class CBrainService : public CBaseService
    {
    public:
        xpointer<CBrain> m_brain;
    };

    MARATHON_ASSERT_OFFSETOF(CBrainService, m_brain, 0x04);
    MARATHON_ASSERT_SIZEOF(CBrainService, 0x08);
}
