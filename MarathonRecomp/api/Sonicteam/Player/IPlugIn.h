#pragma once

#include <Marathon.inl>
#include <stdx/string.h>

namespace Sonicteam::Player
{
    class IPlugIn
    {
    public:
		struct Vftable
		{
			be<uint32_t> fpDestroy;
		};

        xpointer<Vftable> m_pVftable;
        stdx::string m_AttachPoint;
    };

    MARATHON_ASSERT_OFFSETOF(IPlugIn, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(IPlugIn, m_AttachPoint, 0x04);
    MARATHON_ASSERT_SIZEOF(IPlugIn, 0x20);
}
