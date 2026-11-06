#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/data/_acdLeafControl.h>
#include <Acroarts/data/_acdMomList.h>

struct _acdLeaf
{
    _acdLeafControl LeafCtrl;
    be<SpUint32> MomListNum;
    xpointer<xpointer<_acdMomList>> ppMomList;
};

MARATHON_ASSERT_OFFSETOF(_acdLeaf, LeafCtrl, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdLeaf, MomListNum, 0xB4);
MARATHON_ASSERT_OFFSETOF(_acdLeaf, ppMomList, 0xB8);
MARATHON_ASSERT_SIZEOF(_acdLeaf, 0xBC);
