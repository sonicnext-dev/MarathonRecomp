#pragma once

#include <Marathon.inl>
#include <Acroarts/data/res/_acdHMDArray.h>
#include <Acroarts/data/res/_acdHMOArray.h>
#include <Acroarts/data/res/_acdHMTArray.h>
#include <Acroarts/data/res/_acdHTXArray.h>
#include <Acroarts/data/SpTypes.h>

struct _acdResArrayModel
{
    be<SpUint32> HMDNum;
    xpointer<xpointer<_acdHMDArray>> ppHMDArray;
    be<SpUint32> HMTNum;
    xpointer<xpointer<_acdHMTArray>> ppHMTArray;
    be<SpUint32> HMONum;
    xpointer<xpointer<_acdHMOArray>> ppHMOArray;
    be<SpUint32> HTXNum;
    xpointer<xpointer<_acdHTXArray>> ppHTXArray;
};

MARATHON_ASSERT_OFFSETOF(_acdResArrayModel, HMDNum, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdResArrayModel, ppHMDArray, 0x04);
MARATHON_ASSERT_OFFSETOF(_acdResArrayModel, HMTNum, 0x08);
MARATHON_ASSERT_OFFSETOF(_acdResArrayModel, ppHMTArray, 0x0C);
MARATHON_ASSERT_OFFSETOF(_acdResArrayModel, HMONum, 0x10);
MARATHON_ASSERT_OFFSETOF(_acdResArrayModel, ppHMOArray, 0x14);
MARATHON_ASSERT_OFFSETOF(_acdResArrayModel, HTXNum, 0x18);
MARATHON_ASSERT_OFFSETOF(_acdResArrayModel, ppHTXArray, 0x1C);
MARATHON_ASSERT_SIZEOF(_acdResArrayModel, 0x20);
