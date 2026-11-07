#pragma once

#include <Marathon.inl>
#include <Chao/CSD/Core/csdReadFile.h>

namespace Sonicteam
{
    class CReadFileMarathon : public Chao::CSD::CReadFile {};

    MARATHON_ASSERT_SIZEOF(CReadFileMarathon, 0x20);
}
