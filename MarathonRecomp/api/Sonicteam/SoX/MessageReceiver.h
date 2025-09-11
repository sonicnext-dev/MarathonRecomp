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
            be<uint32_t> Destroy;
            be<uint32_t> OnMessageRecieved;
        };

        xpointer<Vftable> m_pVftable;

        bool OnMessageRecieved(IMessage* message)
        {
            return GuestToHostFunction<uint32_t>(m_pVftable->OnMessageRecieved, this, message);
        }
    };
}
