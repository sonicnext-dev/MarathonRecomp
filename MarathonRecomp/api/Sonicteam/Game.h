#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class Game : public SoX::MessageReceiver {};

    MARATHON_ASSERT_SIZEOF(Game, 4);
}
