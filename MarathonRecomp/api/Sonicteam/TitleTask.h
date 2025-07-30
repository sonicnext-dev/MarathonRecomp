#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam
{
    class TitleTask : public Sonicteam::SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x0C);
        be<uint32_t> m_SelectedIndex;
    };
}
