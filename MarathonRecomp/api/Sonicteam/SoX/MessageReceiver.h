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

        bool OnMessageReceived(IMessage* pMessage)
        {
            return GuestToHostFunction<uint32_t>(m_pVftable->fpOnMessageReceived, this, pMessage);
        }
    };
}
