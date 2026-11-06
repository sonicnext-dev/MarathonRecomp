#pragma once

#include <Marathon.inl>
#include <Acroarts/kine/_ackBase.h>
#include <Acroarts/kine/_ackMomMaker.h>

class AckMomServer : public _ackBase
{
public:
    xpointer<_ackMomMaker> _pMomList[5];
};

MARATHON_ASSERT_OFFSETOF(AckMomServer, _pMomList, 0x10);
MARATHON_ASSERT_SIZEOF(AckMomServer, 0x24);
