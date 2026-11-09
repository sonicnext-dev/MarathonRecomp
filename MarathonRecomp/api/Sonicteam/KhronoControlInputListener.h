#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Input/Listener.h>

namespace Sonicteam
{
    class KhronoControlInputListener : public SoX::Input::Listener
    {
    public:
        MARATHON_INSERT_PADDING(0x0C);
    };

    MARATHON_ASSERT_SIZEOF(KhronoControlInputListener, 0x24);
}
