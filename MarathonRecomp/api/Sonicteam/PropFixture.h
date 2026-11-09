#pragma once

#include <Marathon.inl>
#include <Sonicteam/Prop/EntityHandle.h>
#include <Sonicteam/Fixture.h>

namespace Sonicteam
{
    class PropFixture : public Fixture
    {
    public:
        xpointer<Prop::EntityHandle> m_pEntityHandle;
        MARATHON_INSERT_PADDING(0x0C);
    };

    MARATHON_ASSERT_OFFSETOF(PropFixture, m_pEntityHandle, 0x170);
    MARATHON_ASSERT_SIZEOF(PropFixture, 0x180);
}
