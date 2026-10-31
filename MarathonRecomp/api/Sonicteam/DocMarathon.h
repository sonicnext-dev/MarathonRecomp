#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Engine/DocMode.h>
#include <Sonicteam/DocState.h>
#include <Sonicteam/StateMainMode.h>
#include <Sonicteam/TextBook.h>

namespace Sonicteam
{
    class DocMarathon : public SoX::Engine::Doc
    {
    public:
        struct Vftable : public Doc::Vftable
        {
            MARATHON_INSERT_PADDING(0x48);
            be<uint32_t> fpSetDocState;
            MARATHON_INSERT_PADDING(0x0C);
            be<uint32_t> fpGetTextBookContext;
            MARATHON_INSERT_PADDING(0x60);
            be<uint32_t> fpSetLoading;
            MARATHON_INSERT_PADDING(0x44);
            be<uint32_t> fpGetLanguage;
            MARATHON_INSERT_PADDING(0x4C);
            be<uint32_t> fpStartMainMode;
        };

        DocMarathon* SetDocState(DocState* in_pDocState)
        {
            return GuestToHostFunction<DocMarathon*>(static_cast<Vftable*>(m_pVftable.get())->fpSetDocState, this, in_pDocState);
        }

        TextBook::TextBookContext* GetTextBookContext()
        {
            return GuestToHostFunction<TextBook::TextBookContext*>(static_cast<Vftable*>(m_pVftable.get())->fpGetTextBookContext, this);
        }

        void SetLoading(bool in_isLoading)
        {
            GuestToHostFunction<void>(static_cast<Vftable*>(m_pVftable.get())->fpSetLoading, this, in_isLoading);
        }

        const char* GetLanguage()
        {
            return GuestToHostFunction<const char*>(static_cast<Vftable*>(m_pVftable.get())->fpGetLanguage, this);
        }

        DocMarathon* StartMainMode()
        {
            return GuestToHostFunction<DocMarathon*>(static_cast<Vftable*>(m_pVftable.get())->fpStartMainMode, this);
        }
    };

    MARATHON_ASSERT_OFFSETOF(DocMarathon::Vftable, fpSetDocState, 0x54);
    MARATHON_ASSERT_OFFSETOF(DocMarathon::Vftable, fpGetTextBookContext, 0x64);
    MARATHON_ASSERT_OFFSETOF(DocMarathon::Vftable, fpSetLoading, 0xC8);
    MARATHON_ASSERT_OFFSETOF(DocMarathon::Vftable, fpGetLanguage, 0x110);
    MARATHON_ASSERT_OFFSETOF(DocMarathon::Vftable, fpStartMainMode, 0x160);
    
    MARATHON_ASSERT_SIZEOF(DocMarathon, 0x5C);
}
