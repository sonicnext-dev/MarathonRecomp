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
        MARATHON_INSERT_PADDING(0x64);
    };

    MARATHON_ASSERT_OFFSETOF(MessageWindowTask, m_Field90, 0x90);
    MARATHON_ASSERT_OFFSETOF(MessageWindowTask, m_Field98, 0x98);
    MARATHON_ASSERT_SIZEOF(MessageWindowTask, 0x104);
}
