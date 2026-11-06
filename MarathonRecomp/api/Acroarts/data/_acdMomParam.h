#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/data/_acdMomParamSet.h>

union _acdMomParam
{
    be<SpUint32> acd_u32;
    be<SpSint32> acd_s32;
    be<SpAngle> acd_ang;
    be<SpFloat32> acd_f32;
    xpointer<_acdMomParamSet> pParamSet;
};

MARATHON_ASSERT_SIZEOF(_acdMomParam, 0x04);
