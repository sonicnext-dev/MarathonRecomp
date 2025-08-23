#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class HUDMainDisplay : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x24);
        be<Character> m_Character;
    };
}
