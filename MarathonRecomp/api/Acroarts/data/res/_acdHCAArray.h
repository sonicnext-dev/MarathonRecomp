#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdHCAArray
{
    be<SpUint32> Index;
};

MARATHON_ASSERT_OFFSETOF(_acdHCAArray, Index, 0x00);
MARATHON_ASSERT_SIZEOF(_acdHCAArray, 0x04);
