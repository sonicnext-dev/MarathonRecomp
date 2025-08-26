#pragma once

namespace Sonicteam::SoX
{
    struct Message;

    class MessageReceiver
    {
    public:
        struct Vftable
        {
            be<uint32_t> OnMessageRecieved;
        };

        xpointer<Vftable> m_pVftable;

        bool OnMessageRecieved(Message* message)
        {
            return GuestToHostFunction<uint32_t>(m_pVftable->OnMessageRecieved, this, message);
        }
    };
}
