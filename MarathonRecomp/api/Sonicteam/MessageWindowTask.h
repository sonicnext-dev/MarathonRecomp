#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class MessageWindowTask : public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x44);
        boost::shared_ptr<TextEntity> m_Field90;
        boost::shared_ptr<TextEntity> m_Field98;
    };
}
