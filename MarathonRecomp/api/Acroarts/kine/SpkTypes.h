#pragma once

#include <Acroarts/data/SpTypes.h>

using SpkBlendType = SpUint32;
using SpkDrawActionType = SpUint8;
using SpkMotionType = SpUint32;

enum SPK_COLOR_BLEND_MODE : SpSint32
{
    SPK_COLOR_BLEND_MODULATION,
    SPK_COLOR_BLEND_ADDITION,
    SPK_COLOR_BLEND_SUBTRACTION,
    SPK_COLOR_BLEND_REPLACE
};
