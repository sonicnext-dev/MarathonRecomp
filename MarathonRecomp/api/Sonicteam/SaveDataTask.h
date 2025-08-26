#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class SaveDataTask : public SoX::Engine::Task
    {
    public:
        xpointer<const char> m_pFileName;
        MARATHON_INSERT_PADDING(0x24);
        be<uint32_t> m_Flags;
        MARATHON_INSERT_PADDING(4);
    };
}
