#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message
{
    struct MsgCameramanCameraInitialize : SoX::Message<0x14007>
    {
        be<uint32_t> m_PadID;
        be<uint32_t> m_TargetActorID;
        bool IsDemoCam;
    };
}
