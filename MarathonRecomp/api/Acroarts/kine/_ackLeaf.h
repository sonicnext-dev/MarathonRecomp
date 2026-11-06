#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/data/_acdLeafControl.h>
#include <Acroarts/kine/AckDataInst.h>
#include <Acroarts/kine/SpkTypes.h>
#include <Acroarts/kine/_ackBase.h>
#include <Acroarts/kine/_ackBranchStatus.h>
#include <Acroarts/kine/_ackLeafStatus.h>
#include <Acroarts/kine/_ackMomBase.h>
#include <Acroarts/kine/_ackTrunkStatus.h>

class _ackLeaf : public _ackBase
{
public:
    SpMatrix _LeafMatrix;
    _ackLeafStatus _status;
    xpointer<_acdLeafControl> _pCtrl;
    xpointer<_ackLeaf> _pPrev;
    xpointer<_ackLeaf> _pNext;
    xpointer<_ackLeaf> _pParent;
    xpointer<_ackLeaf> _pChild;
    xpointer<_ackLeaf> _pAttach;
    be<SpFloat32> _myCount;
    be<SpFloat32> _genCount;
    be<SpUint32> _Depth;
    xpointer<_ackTrunkStatus> _pTrunkStatus;
    xpointer<_ackBranchStatus> _pBranchStatus;
    xpointer<AckDataInst> _pDataInst;
    be<SpFloat32> _myLifeTime;
    xpointer<_ackMomBase> _pMomInstList;
    xpointer<SpMatrix> _pMlsMatrix;
    xpointer<SpMatrix> _pCoordMatrix;
    xpointer<SpMatrix> _pDetachMatrix;
    be<SpSint16> _MomListId;
    be<SpUint16> _Flags;
};

MARATHON_ASSERT_OFFSETOF(_ackLeaf, _LeafMatrix, 0x10);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _status, 0x50);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _pCtrl, 0x68);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _pPrev, 0x6C);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _pNext, 0x70);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _pParent, 0x74);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _pChild, 0x78);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _pAttach, 0x7C);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _myCount, 0x80);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _genCount, 0x84);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _Depth, 0x88);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _pTrunkStatus, 0x8C);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _pBranchStatus, 0x90);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _pDataInst, 0x94);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _myLifeTime, 0x98);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _pMomInstList, 0x9C);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _pMlsMatrix, 0xA0);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _pCoordMatrix, 0xA4);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _pDetachMatrix, 0xA8);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _MomListId, 0xAC);
MARATHON_ASSERT_OFFSETOF(_ackLeaf, _Flags, 0xAE);
MARATHON_ASSERT_SIZEOF(_ackLeaf, 0xB0);
