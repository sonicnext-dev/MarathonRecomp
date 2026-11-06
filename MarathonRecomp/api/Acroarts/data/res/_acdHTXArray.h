#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdHTXArray
{
    be<SpUint32> Index;
};

MARATHON_ASSERT_OFFSETOF(_acdHTXArray, Index, 0x00);
MARATHON_ASSERT_SIZEOF(_acdHTXArray, 0x04);
