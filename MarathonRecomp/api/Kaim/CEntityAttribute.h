#pragma once

#include <Marathon.inl>
#include <Kaim/CObject.h>

namespace Kaim
{
    class CEntityAttribute : public CObject {};

    MARATHON_ASSERT_SIZEOF(CEntityAttribute, 0x04);
}
