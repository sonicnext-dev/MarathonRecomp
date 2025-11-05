#pragma once

#include <Marathon.inl>

namespace Sonicteam::Mission
{
    class Core : public SoX::MessageReceiver
    {
    public:
        MARATHON_INSERT_PADDING(0xC81C);
    };

    MARATHON_ASSERT_SIZEOF(Core, 0xC820);
}
