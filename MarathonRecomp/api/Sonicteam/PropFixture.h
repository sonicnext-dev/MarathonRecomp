#pragma once

#include <Marathon.inl>
#include <Sonicteam/Fixture.h>

namespace Sonicteam
{
    class PropFixture : public Fixture
    {
    public:
        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_SIZEOF(PropFixture, 0x180);
}
