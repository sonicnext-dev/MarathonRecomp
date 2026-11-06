#pragma once

#include <Sonicteam/SoX/Math/Matrix.h>
#include <Sonicteam/SoX/Color.h>

using SpSint8 = int8_t;
using SpUint8 = uint8_t;
using SpSint16 = int16_t;
using SpUint16 = uint16_t;
using SpSint32 = int32_t;
using SpUint32 = uint32_t;
using SpSint64 = int64_t;
using SpUint64 = uint64_t;
using SpFloat32 = float;

using AbdErrHandle = SpUint32;
using AbdResHandle = SpUint32;

using SpAddr = xpointer<void>;
using SpAngle = SpSint32;
using SpBool = SpUint32;
using SpColorReal = Sonicteam::SoX::RGBAF32;
using SpMatrix = Sonicteam::SoX::Math::Matrix4x4;
