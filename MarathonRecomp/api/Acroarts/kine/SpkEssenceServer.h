#pragma once

#include <Marathon.inl>
#include <Acroarts/kine/SpkBase.h>
#include <Acroarts/kine/_spkEssenceMaker.h>

class SpkEssenceServer : public SpkBase
{
public:
    xpointer<_spkEssenceMaker> _pEssenceList[5];
};

MARATHON_ASSERT_OFFSETOF(SpkEssenceServer, _pEssenceList, 0x10);
MARATHON_ASSERT_SIZEOF(SpkEssenceServer, 0x24);
