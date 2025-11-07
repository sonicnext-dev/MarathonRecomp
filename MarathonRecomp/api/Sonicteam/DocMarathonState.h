#pragma once

#include <Marathon.inl>
#include <Sonicteam/DocMarathonImp.h>

namespace Sonicteam
{
    class DocMarathonState : public DocMarathonImp {};

    MARATHON_ASSERT_SIZEOF(DocMarathonState, 0x55C3C);
}
