#pragma once

#include <Marathon.inl>
#include <Kaim/CData.h>

namespace Kaim
{
    class CEntityDefinition : public CData {};

    MARATHON_ASSERT_SIZEOF(CEntityDefinition, 0x34);
}
