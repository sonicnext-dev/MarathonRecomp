#pragma once

namespace Sonicteam
{
    class PauseAdapter : public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> m_SelectedIndex;
    };
}
