#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/data/_acdBranch.h>
#include <Acroarts/data/_acdHeader.h>
#include <Acroarts/data/_acdLeaf.h>
#include <Acroarts/data/_acdMom.h>
#include <Acroarts/data/_acdTrunk.h>
#include <Acroarts/kine/SpkEssenceBase.h>
#include <Acroarts/kine/SpkEssenceServer.h>
#include <Acroarts/kine/_ackBase.h>

class AckDataInst : public _ackBase
{
public:
    xpointer<_acdHeader> _pDatHeader;
    xpointer<_acdTrunk> _pCurTrunk;
    xpointer<_acdBranch> _pCurBranch;
    xpointer<_acdLeaf> _pCurLeaf;
    xpointer<_acdMom> _pCurMom;
    xpointer<SpkEssenceServer> _pEsServer;
    xpointer<xpointer<SpkEssenceBase>> _ppEsInst;
    xpointer<be<AbdResHandle>> _pResHandle;
    xpointer<be<SpSint32>> _pVResId;
    xpointer<SpUint8> _pResFlags;
    be<SpUint32> _NumResInst;
    be<AbdResHandle> _NumRes;
    be<SpSint32> _DataID;
    be<SpSint32> _DataSubID;
    be<AbdErrHandle> _errorCode;
};

MARATHON_ASSERT_OFFSETOF(AckDataInst, _pDatHeader, 0x10);
MARATHON_ASSERT_OFFSETOF(AckDataInst, _pCurTrunk, 0x14);
MARATHON_ASSERT_OFFSETOF(AckDataInst, _pCurBranch, 0x18);
MARATHON_ASSERT_OFFSETOF(AckDataInst, _pCurLeaf, 0x1C);
MARATHON_ASSERT_OFFSETOF(AckDataInst, _pCurMom, 0x20);
MARATHON_ASSERT_OFFSETOF(AckDataInst, _pEsServer, 0x24);
MARATHON_ASSERT_OFFSETOF(AckDataInst, _ppEsInst, 0x28);
MARATHON_ASSERT_OFFSETOF(AckDataInst, _pResHandle, 0x2C);
MARATHON_ASSERT_OFFSETOF(AckDataInst, _pVResId, 0x30);
MARATHON_ASSERT_OFFSETOF(AckDataInst, _pResFlags, 0x34);
MARATHON_ASSERT_OFFSETOF(AckDataInst, _NumResInst, 0x38);
MARATHON_ASSERT_OFFSETOF(AckDataInst, _NumRes, 0x3C);
MARATHON_ASSERT_OFFSETOF(AckDataInst, _DataID, 0x40);
MARATHON_ASSERT_OFFSETOF(AckDataInst, _DataSubID, 0x44);
MARATHON_ASSERT_OFFSETOF(AckDataInst, _errorCode, 0x48);
MARATHON_ASSERT_SIZEOF(AckDataInst, 0x4C);
