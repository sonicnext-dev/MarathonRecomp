#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdHSPArray
{
    be<SpUint32> Index;
    be<SpUint32> DefaultAction;
};

MARATHON_ASSERT_OFFSETOF(_acdHSPArray, Index, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdHSPArray, DefaultAction, 0x04);
MARATHON_ASSERT_SIZEOF(_acdHSPArray, 0x08);
