#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/kine/AckDataInst.h>
#include <Acroarts/kine/AckMomServer.h>
#include <Acroarts/kine/SpkSpangleBase.h>

struct AckLightServer;

struct _ackTrunkStatus
{
    SpMatrix MasterMatrix;
    SpMatrix CurrentMatrix;
    xpointer<AckDataInst> pDataInst;
    xpointer<AckLightServer> pLightServer;
    xpointer<AckMomServer> pMomServer;
    xpointer<SpkSpangleBase> pCoordSpangle;
    xpointer<void> Callback;
    be<SpFloat32> DefaultTimeRate;
    be<SpFloat32> TimeRate;
    be<SpFloat32> TimeRateBak;
    be<SpBool> IsOffsetColor;
    SpColorReal OffsetColor;
    be<SpBool> IsCurrentMatrix;
    be<SpBool> IsMomDebugFlag;
    be<SpBool> IsSortManage;
    be<SpBool> IsExec;
};

MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, MasterMatrix, 0x00);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, CurrentMatrix, 0x40);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, pDataInst, 0x80);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, pLightServer, 0x84);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, pMomServer, 0x88);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, pCoordSpangle, 0x8C);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, Callback, 0x90);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, DefaultTimeRate, 0x94);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, TimeRate, 0x98);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, TimeRateBak, 0x9C);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, IsOffsetColor, 0xA0);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, OffsetColor, 0xA4);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, IsCurrentMatrix, 0xB4);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, IsMomDebugFlag, 0xB8);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, IsSortManage, 0xBC);
MARATHON_ASSERT_OFFSETOF(_ackTrunkStatus, IsExec, 0xC0);
MARATHON_ASSERT_SIZEOF(_ackTrunkStatus, 0xC4);
