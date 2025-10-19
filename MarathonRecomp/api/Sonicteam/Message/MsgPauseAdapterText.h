#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Message.h>
#include <stdx/string.h>

namespace Sonicteam::Message
{
    struct MsgPauseAdapterText : SoX::Message<0x1C003>
    {
        stdx::string PauseName;
        stdx::string SelectedName;
    };
}
