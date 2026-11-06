#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/data/_acdLeafControl.h>
#include <Acroarts/data/_acdMomControl.h>
#include <Acroarts/data/_acdMomParam.h>
#include <Acroarts/kine/AckDataInst.h>
#include <Acroarts/kine/_ackBase.h>
#include <Acroarts/kine/_ackBranchStatus.h>
#include <Acroarts/kine/_ackTrunkStatus.h>
#include <Acroarts/kine/_ackLeafStatus.h>

class _ackMomBase : public _ackBase
{
public:
    struct Vftable
    {
        be<uint32_t> fpExecCalling1st;
        be<uint32_t> fpExecCallingNow;
        be<uint32_t> fpExecCallingFinished;
        be<uint32_t> fpExecCallingAfter;
        be<uint32_t> fpExec;
        be<uint32_t> fpDrawCalling1st;
        be<uint32_t> fpDrawCallingNow;
        be<uint32_t> fpDrawCallingFinished;
        be<uint32_t> fpDrawCallingAfter;
        be<uint32_t> fpDraw;
        be<uint32_t> fpControlCallback;
        be<uint32_t> fpSetExternControlParam;
        be<uint32_t> fpClearExternControlParam;
    };

    xpointer<AckDataInst> _pDataInst;
    xpointer<_acdMomControl> _pCtrl;
    xpointer<_acdLeafControl> _pLeafCtrl;
    be<SpUint32> _flags;
    be<SpFloat32> _myCount;
    xpointer<_ackMomBase> _pNext;
    xpointer<_ackTrunkStatus> _pTrunkStatus;
    xpointer<_ackBranchStatus> _pBranchStatus;
    xpointer<_ackLeafStatus> _pLeafStatus;
    be<SpUint16> _User_Status;
    xpointer<_acdMomParam> _pParam;
};

MARATHON_ASSERT_OFFSETOF(_ackMomBase, _pDataInst, 0x10);
MARATHON_ASSERT_OFFSETOF(_ackMomBase, _pCtrl, 0x14);
MARATHON_ASSERT_OFFSETOF(_ackMomBase, _pLeafCtrl, 0x18);
MARATHON_ASSERT_OFFSETOF(_ackMomBase, _flags, 0x1C);
MARATHON_ASSERT_OFFSETOF(_ackMomBase, _myCount, 0x20);
MARATHON_ASSERT_OFFSETOF(_ackMomBase, _pNext, 0x24);
MARATHON_ASSERT_OFFSETOF(_ackMomBase, _pTrunkStatus, 0x28);
MARATHON_ASSERT_OFFSETOF(_ackMomBase, _pBranchStatus, 0x2C);
MARATHON_ASSERT_OFFSETOF(_ackMomBase, _pLeafStatus, 0x30);
MARATHON_ASSERT_OFFSETOF(_ackMomBase, _User_Status, 0x34);
MARATHON_ASSERT_OFFSETOF(_ackMomBase, _pParam, 0x38);
MARATHON_ASSERT_SIZEOF(_ackMomBase, 0x3C);
