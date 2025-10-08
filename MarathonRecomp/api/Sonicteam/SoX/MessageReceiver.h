#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::SoX
{
    class MessageReceiver
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpProcessMessage;
        };

        xpointer<Vftable> m_pVftable;

        void* Destroy(uint8_t flags = 1)
        {
            return GuestToHostFunction<void*>(m_pVftable->fpDestroy, this, flags);
        }

        bool ProcessMessage(IMessage* pMessage)
        {
            return GuestToHostFunction<bool>(m_pVftable->fpProcessMessage, this, pMessage);
        }

        template <typename Message>
        bool ProcessMessage(Message& message)
        {
            guest_heap_var<Message> msg(message);
            bool result = ProcessMessage((IMessage*)msg.get());
            message = *msg;
            return result;
        }
    };

    MARATHON_ASSERT_OFFSETOF(MessageReceiver, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(MessageReceiver, 4);
}
