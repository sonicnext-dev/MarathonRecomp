#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::State
{
    class IMachine : public IPlugIn
    {
    public:
        struct Vftable : IPlugIn::Vftable
        {
            be<uint32_t> fpUpdate;
            be<uint32_t> fpGetStateContext;
            be<uint32_t> fpChangeState;
            be<uint32_t> fpChangeStateQueue;
            be<uint32_t> fpEnableQueueState;
            be<uint32_t> fpEnableQueueStatePop;
            be<uint32_t> fpChangeStatePop;
            be<uint32_t> fpGetStateID;
        };

        void ChangeState(uint32_t state)
        {
            auto vft = (Vftable*)(m_pVftable.get());
            GuestToHostFunction<void>(vft->fpChangeState, this, state);
        }

        void ChangeStateQueue(uint32_t state)
        {
            auto vft = (Vftable*)(m_pVftable.get());
            GuestToHostFunction<void>(vft->fpChangeStateQueue, this, state);
        }

    };
}
