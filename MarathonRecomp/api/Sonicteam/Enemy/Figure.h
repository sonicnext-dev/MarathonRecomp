#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/MessageReceiver.h>

namespace Sonicteam::Enemy
{
    class Figure : public SoX::MessageReceiver
    {
    public:
        MARATHON_INSERT_PADDING(0x8C);
    };

    MARATHON_ASSERT_SIZEOF(Figure, 0x90);
}
