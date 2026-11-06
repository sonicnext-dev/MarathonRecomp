#pragma once

#include <Marathon.inl>
#include <Acroarts/data/res/_acdHTXArray.h>
#include <Acroarts/data/SpTypes.h>

struct _acdResArrayPrimitive
{
    be<SpUint32> HTXNum;
    xpointer<xpointer<_acdHTXArray>> ppHTXArray;
};

MARATHON_ASSERT_OFFSETOF(_acdResArrayPrimitive, HTXNum, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdResArrayPrimitive, ppHTXArray, 0x04);
MARATHON_ASSERT_SIZEOF(_acdResArrayPrimitive, 0x08);
