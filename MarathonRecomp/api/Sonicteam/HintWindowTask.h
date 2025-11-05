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
        MARATHON_INSERT_PADDING(0x60);
    };

    MARATHON_ASSERT_OFFSETOF(HintWindowTask, m_Field8C, 0x8C);
    MARATHON_ASSERT_OFFSETOF(HintWindowTask, m_Field94, 0x94);
    MARATHON_ASSERT_SIZEOF(HintWindowTask, 0xFC);
}
