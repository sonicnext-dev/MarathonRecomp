#pragma once

#include <Marathon.inl>
#include <Kaim/CObject.h>

namespace Kaim
{
    class CDataReader : public CObject {};

    MARATHON_ASSERT_SIZEOF(CDataReader, 0x04);
}
