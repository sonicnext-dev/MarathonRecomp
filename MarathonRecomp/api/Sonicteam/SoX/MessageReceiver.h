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
            be<uint32_t> fpOnMessageReceived;
        };

        xpointer<Vftable> m_pVftable;

        void Destroy(uint8_t flags = 1)
        {
            GuestToHostFunction<int>(m_pVftable->fpDestroy, this, flags);
        }

        bool OnMessageReceived(IMessage* pMessage)
        {
            return GuestToHostFunction<bool>(m_pVftable->fpOnMessageReceived, this, pMessage);
        }
    };
}
