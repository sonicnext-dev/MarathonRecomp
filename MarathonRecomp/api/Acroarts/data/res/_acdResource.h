#pragma once

#include <Marathon.inl>
#include <Acroarts/data/res/_acdResArrayModel.h>
#include <Acroarts/data/res/_acdResArrayCellSprite.h>
#include <Acroarts/data/res/_acdResArrayCamera.h>
#include <Acroarts/data/res/_acdResArrayScreen.h>
#include <Acroarts/data/res/_acdResArrayLight.h>
#include <Acroarts/data/res/_acdResArrayPrimitive.h>
#include <Acroarts/data/res/_acdResArrayExtra.h>

union _acdResource
{
    _acdResArrayModel ResArrayModel;
    _acdResArrayCellSprite ResArrayCellSprite;
    _acdResArrayCamera ResArrayCamera;
    _acdResArrayScreen ResArrayScreen;
    _acdResArrayLight ResArrayLight;
    _acdResArrayPrimitive ResArrayPrimitive;
    _acdResArrayExtra ResArrayExtra;
};

MARATHON_ASSERT_SIZEOF(_acdResource, 0x20);
