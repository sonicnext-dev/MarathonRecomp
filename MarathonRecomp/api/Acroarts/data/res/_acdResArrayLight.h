#pragma once

#include <Marathon.inl>
#include <Acroarts/data/res/_acdHLIArray.h>
#include <Acroarts/data/res/_acdHLMArray.h>
#include <Acroarts/data/SpTypes.h>

struct _acdResArrayLight
{
    be<SpUint32> HLINum;
    xpointer<xpointer<_acdHLIArray>> ppHLIArray;
    be<SpUint32> HLMNum;
    xpointer<xpointer<_acdHLMArray>> ppHLMArray;
};

MARATHON_ASSERT_OFFSETOF(_acdResArrayLight, HLINum, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdResArrayLight, ppHLIArray, 0x04);
MARATHON_ASSERT_OFFSETOF(_acdResArrayLight, HLMNum, 0x08);
MARATHON_ASSERT_OFFSETOF(_acdResArrayLight, ppHLMArray, 0x0C);
MARATHON_ASSERT_SIZEOF(_acdResArrayLight, 0x10);
