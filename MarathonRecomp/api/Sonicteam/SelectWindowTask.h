#pragma once

#include <Marathon.inl>
#include <Sonicteam/HUDCALLBACK.h>

namespace Sonicteam
{
    class SelectWindowTask : public SoX::Engine::Task, public HUDCALLBACK
    {
    public:
        MARATHON_INSERT_PADDING(0x40);
        be<uint32_t> m_ChosenIndex;
        MARATHON_INSERT_PADDING(0x28);
        be<uint32_t> m_SelectedIndex;
    };
}
