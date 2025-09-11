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
            be<uint32_t> fpOnMessageRecieved;
        };

        xpointer<Vftable> m_pVftable;

        bool OnMessageRecieved(IMessage* pMessage)
        {
            return GuestToHostFunction<uint32_t>(m_pVftable->fpOnMessageRecieved, this, pMessage);
        }
    };
}
