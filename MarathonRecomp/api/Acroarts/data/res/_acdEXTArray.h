#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdEXTArray
{
    be<SpUint32> Index;
};

MARATHON_ASSERT_OFFSETOF(_acdEXTArray, Index, 0x00);
MARATHON_ASSERT_SIZEOF(_acdEXTArray, 0x04);
