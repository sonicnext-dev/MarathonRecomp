#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message
{
    struct MsgSuckPlayer : SoX::Message<0x11034A>
    {
        Sonicteam::SoX::Math::Vector Point;
    };
}
