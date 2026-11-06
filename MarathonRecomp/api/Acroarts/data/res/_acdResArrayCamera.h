#pragma once

#include <Marathon.inl>
#include <Acroarts/data/res/_acdHCAArray.h>
#include <Acroarts/data/res/_acdHCMArray.h>
#include <Acroarts/data/SpTypes.h>

struct _acdResArrayCamera
{
    be<SpUint32> HCANum;
    xpointer<xpointer<_acdHCAArray>> ppHCAArray;
    be<SpUint32> HCMNum;
    xpointer<xpointer<_acdHCMArray>> ppHCMArray;
};

MARATHON_ASSERT_OFFSETOF(_acdResArrayCamera, HCANum, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdResArrayCamera, ppHCAArray, 0x04);
MARATHON_ASSERT_OFFSETOF(_acdResArrayCamera, HCMNum, 0x08);
MARATHON_ASSERT_OFFSETOF(_acdResArrayCamera, ppHCMArray, 0x0C);
MARATHON_ASSERT_SIZEOF(_acdResArrayCamera, 0x10);
