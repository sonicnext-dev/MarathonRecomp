#pragma once

#include <Marathon.inl>
#include <Sonicteam/Actor.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class NamedActor : public Actor
    {
    public:
        stdx::string m_AttachPoint;
    };

    MARATHON_ASSERT_OFFSETOF(NamedActor, m_AttachPoint, 0x58);
    MARATHON_ASSERT_SIZEOF(NamedActor, 0x74);
}
