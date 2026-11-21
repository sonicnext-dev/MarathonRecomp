#pragma once

#include <Marathon.inl>
#include <Kaim/CBaseService.h>

namespace Kaim
{
    class CWorldService : public CBaseService {};

    MARATHON_ASSERT_SIZEOF(CWorldService, 0x04);
}
