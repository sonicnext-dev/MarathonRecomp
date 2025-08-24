#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class HUDLoading : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x5C);
        be<uint32_t> m_Flags;
    };
}
