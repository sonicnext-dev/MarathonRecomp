#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/data/_acdMom.h>
#include <Acroarts/data/_acdMomListControl.h>

struct _acdMomList
{
    _acdMomListControl MomListCtrl;
    be<SpUint32> MomNum;
    xpointer<xpointer<_acdMom>> ppMom;
};

MARATHON_ASSERT_OFFSETOF(_acdMomList, MomListCtrl, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdMomList, MomNum, 0x14);
MARATHON_ASSERT_OFFSETOF(_acdMomList, ppMom, 0x18);
MARATHON_ASSERT_SIZEOF(_acdMomList, 0x1C);
