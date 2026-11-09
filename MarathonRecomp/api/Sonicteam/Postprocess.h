#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam
{
    class PostprocessController;

    class Postprocess : public SoX::RefCountObject
    {
    public:
        struct Vftable : public SoX::RefCountObject::Vftable
        {
            be<uint32_t> fpGetName;
            MARATHON_INSERT_PADDING(0x04);
            be<uint32_t> fpExecute;
        };

        xpointer<SoX::Engine::Doc> m_pDoc;
        xpointer<PostprocessController> m_pPostprocessController;
        SoX::LinkNode<Postprocess> m_Siblings;

        const char* GetName()
        {
            return GuestToHostFunction<const char*>(static_cast<Vftable*>(m_pVftable.get())->fpGetName.get(), this);
        }
    };

    MARATHON_ASSERT_OFFSETOF(Postprocess::Vftable, fpGetName, 0x04);
    MARATHON_ASSERT_OFFSETOF(Postprocess::Vftable, fpExecute, 0x0C);
    MARATHON_ASSERT_SIZEOF(Postprocess::Vftable, 0x10);

    MARATHON_ASSERT_OFFSETOF(Postprocess, m_pDoc, 0x08);
    MARATHON_ASSERT_OFFSETOF(Postprocess, m_pPostprocessController, 0x0C);
    MARATHON_ASSERT_OFFSETOF(Postprocess, m_Siblings, 0x10);
    MARATHON_ASSERT_SIZEOF(Postprocess, 0x1C);
}
