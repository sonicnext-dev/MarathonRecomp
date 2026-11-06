#pragma once

#include <Marathon.inl>
#include <Acroarts/data/res/_acdEXTArray.h>
#include <Acroarts/data/SpTypes.h>

struct _acdResArrayExtra
{
    be<SpUint32> EXTNum;
    xpointer<xpointer<_acdEXTArray>> ppEXTArray;
};

MARATHON_ASSERT_OFFSETOF(_acdResArrayExtra, EXTNum, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdResArrayExtra, ppEXTArray, 0x04);
MARATHON_ASSERT_SIZEOF(_acdResArrayExtra, 0x08);
