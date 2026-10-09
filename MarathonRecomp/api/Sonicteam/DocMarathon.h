#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>

namespace Sonicteam
{
    class DocMarathon : public SoX::Engine::Doc
    {
    public:
        struct Vftable : public SoX::Engine::Doc::Vftable
        {
            MARATHON_INSERT_PADDING(0xBC);
            be<uint32_t> fpSetLoading;
        };

        void SetLoading(bool isLoading)
        {
            GuestToHostFunction<void>(static_cast<Vftable*>(m_pVftable.get())->fpSetLoading, this, isLoading);
        }
    };
}
