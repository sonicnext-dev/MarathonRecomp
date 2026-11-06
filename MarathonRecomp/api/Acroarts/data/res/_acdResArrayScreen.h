#pragma once

#include <Marathon.inl>
#include <Acroarts/data/res/_acdHSCArray.h>
#include <Acroarts/data/res/_acdHSMArray.h>
#include <Acroarts/data/SpTypes.h>

struct _acdResArrayScreen
{
    be<SpUint32> HSCNum;
    xpointer<xpointer<_acdHSCArray>> ppHSCArray;
    be<SpUint32> HSMNum;
    xpointer<xpointer<_acdHSMArray>> ppHSMArray;
};

MARATHON_ASSERT_OFFSETOF(_acdResArrayScreen, HSCNum, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdResArrayScreen, ppHSCArray, 0x04);
MARATHON_ASSERT_OFFSETOF(_acdResArrayScreen, HSMNum, 0x08);
MARATHON_ASSERT_OFFSETOF(_acdResArrayScreen, ppHSMArray, 0x0C);
MARATHON_ASSERT_SIZEOF(_acdResArrayScreen, 0x10);
