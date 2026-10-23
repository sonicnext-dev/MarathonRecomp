#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/TextCard.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class StageTitleTask : public SoX::Engine::Task
    {
    public:
        be<uint32_t> m_State;
        stdx::string m_Name;
        boost::shared_ptr<TextCard> m_spTextCard;
        MARATHON_INSERT_PADDING(0x0C);
    };

    MARATHON_ASSERT_OFFSETOF(StageTitleTask, m_State, 0x4C);
    MARATHON_ASSERT_OFFSETOF(StageTitleTask, m_Name, 0x50);
    MARATHON_ASSERT_OFFSETOF(StageTitleTask, m_spTextCard, 0x6C);
    MARATHON_ASSERT_SIZEOF(StageTitleTask, 0x80);
}
