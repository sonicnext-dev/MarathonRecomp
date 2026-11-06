#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdHLMArray
{
    be<SpUint32> Index;
};

MARATHON_ASSERT_OFFSETOF(_acdHLMArray, Index, 0x00);
MARATHON_ASSERT_SIZEOF(_acdHLMArray, 0x04);
