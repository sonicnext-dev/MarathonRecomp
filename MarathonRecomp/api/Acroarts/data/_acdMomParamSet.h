#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

union _acdMomParam;

struct _acdMomParamSet
{
    be<SpUint32> ParamNum;
    xpointer<_acdMomParam> pParam;
};

MARATHON_ASSERT_OFFSETOF(_acdMomParamSet, ParamNum, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdMomParamSet, pParam, 0x04);
MARATHON_ASSERT_SIZEOF(_acdMomParamSet, 0x08);
