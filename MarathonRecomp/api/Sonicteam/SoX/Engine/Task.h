#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Component.h>
#include <Sonicteam/SoX/MessageReceiver.h>

namespace Sonicteam::SoX::Engine
{
    class Task : public Component, public MessageReceiver
    {
    public:
        MARATHON_INSERT_PADDING(0x28);
    };
}
