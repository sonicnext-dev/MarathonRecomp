#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/MessageReceiver.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Physics/Shape.h>

namespace Sonicteam::SoX::Physics
{
    class Entity : public SoX::MessageReceiver, public SoX::RefCountObject
    {
    public:
        MARATHON_INSERT_PADDING(0x14);
        xpointer<SoX::MessageReceiver> m_Receiver;
        xpointer<SoX::Physics::Shape> m_RefShape; //Reference_type
    };
}
