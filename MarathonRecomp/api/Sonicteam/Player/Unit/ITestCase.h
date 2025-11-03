#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::Unit
{
    class ITestCase
	{
    public:
        xpointer<void> m_pVftable;
	};

    MARATHON_ASSERT_OFFSETOF(ITestCase, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(ITestCase, 4);
}
