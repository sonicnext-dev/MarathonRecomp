#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Component.h>
#include <Sonicteam/SoX/MessageReceiver.h>

namespace Sonicteam::SoX::Engine
{
    class GTask : public Component, public MessageReceiver
    {
    public:
        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_SIZEOF(GTask, 0x34);
}
