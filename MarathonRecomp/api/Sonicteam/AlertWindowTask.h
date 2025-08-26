#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/SelectWindowTask.h>

namespace Sonicteam
{
    class AlertWindowTask : public SoX::Engine::Task
    {
    public:
        be<uint32_t> m_HasSelectWindow;
        MARATHON_INSERT_PADDING(0x2C);
        be<uint32_t> m_Operation;
        MARATHON_INSERT_PADDING(0x14);
        boost::anonymous_shared_ptr m_spOptions[3];
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> m_OptionCount;
        xpointer<SelectWindowTask> m_pSelectWindowTask;
    };
}
