#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam
{
    class TitleTask : public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x30);
        be<uint32_t> m_SelectedIndex;
    };
}
