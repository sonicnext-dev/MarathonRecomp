#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class HintWindowTask : public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x40);
        boost::shared_ptr<TextEntity> m_Field8C;
        boost::shared_ptr<TextEntity> m_Field94;
    };
}
