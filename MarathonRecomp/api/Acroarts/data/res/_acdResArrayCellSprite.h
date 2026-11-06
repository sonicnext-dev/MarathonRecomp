#pragma once

#include <Marathon.inl>
#include <Acroarts/data/res/_acdHSPArray.h>
#include <Acroarts/data/res/_acdHTXArray.h>
#include <Acroarts/data/SpTypes.h>

struct _acdResArrayCellSprite
{
    be<SpUint32> HSPNum;
    xpointer<xpointer<_acdHSPArray>> ppHSPArray;
    be<SpUint32> HTXNum;
    xpointer<xpointer<_acdHTXArray>> ppHTXArray;
};

MARATHON_ASSERT_OFFSETOF(_acdResArrayCellSprite, HSPNum, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdResArrayCellSprite, ppHSPArray, 0x04);
MARATHON_ASSERT_OFFSETOF(_acdResArrayCellSprite, HTXNum, 0x08);
MARATHON_ASSERT_OFFSETOF(_acdResArrayCellSprite, ppHTXArray, 0x0C);
MARATHON_ASSERT_SIZEOF(_acdResArrayCellSprite, 0x10);
