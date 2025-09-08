#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message
{
    struct MsgObjJump123GetNextPoint : SoX::Message<0x10007>
    {
        Sonicteam::SoX::Math::Quaternion m_Rotation;
		Sonicteam::SoX::Math::Vector m_Position;
    };
}
