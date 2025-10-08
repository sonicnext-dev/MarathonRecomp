#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::SoX::AI
{
    template <typename TStateContext>
    class State
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpGetName;
            be<uint32_t> fpOnStart;
            be<uint32_t> fpOnUpdate;
            be<uint32_t> fpOnEnd;
            be<uint32_t> fpProcessMessage;
        };

        xpointer<Vftable> m_pVftable;

        const char* GetName()
        {
            return GuestToHostFunction<const char*>(m_pVftable->fpGetName, this);
        }

        void OnStart()
        {
            return GuestToHostFunction<const char*>(m_pVftable->fpOnStart, this);
        }

        void OnUpdate(double delta)
        {
            return GuestToHostFunction<const char*>(m_pVftable->fpOnUpdate, this, delta);
        }

        void OnEnd()
        {
            return GuestToHostFunction<const char*>(m_pVftable->fpOnEnd, this);
        }

        bool ProcessMessage(void* ptr,SoX::IMessage* message)
        {
            return GuestToHostFunction<const char*>(m_pVftable->fpProcessMessage, this, ptr,message);
        }

    };
}
