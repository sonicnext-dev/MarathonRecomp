#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/data/_acdMomControl.h>
#include <Acroarts/data/_acdMomParam.h>

struct _acdMom
{
    _acdMomControl MomCtrl;
    be<SpUint32> ParamNum;
    xpointer<_acdMomParam> pParam;
};

MARATHON_ASSERT_OFFSETOF(_acdMom, MomCtrl, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdMom, ParamNum, 0x14);
MARATHON_ASSERT_OFFSETOF(_acdMom, pParam, 0x18);
MARATHON_ASSERT_SIZEOF(_acdMom, 0x1C);
