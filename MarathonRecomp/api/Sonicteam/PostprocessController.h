#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/MyTexture.h>
#include <stdx/vector.h>

namespace Sonicteam
{
    class PostprocessController
    {
    public:
        xpointer<void> m_pVftable;
        xpointer<SoX::Engine::Doc> m_pDoc;
        xpointer<PostprocessController> m_pPrev;
        xpointer<PostprocessController> m_pNext;
        MARATHON_INSERT_PADDING(0x04);
        stdx::vector<SoX::RefSharedPointer<MyTexture>> m_RenderTargetTextures;
        stdx::vector<SoX::RefSharedPointer<MyTexture>> m_DepthStencilTextures;
        MARATHON_INSERT_PADDING(0x08);
    };

    MARATHON_ASSERT_OFFSETOF(PostprocessController, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(PostprocessController, m_pDoc, 0x04);
    MARATHON_ASSERT_OFFSETOF(PostprocessController, m_pPrev, 0x08);
    MARATHON_ASSERT_OFFSETOF(PostprocessController, m_pNext, 0x0C);
    MARATHON_ASSERT_OFFSETOF(PostprocessController, m_RenderTargetTextures, 0x14);
    MARATHON_ASSERT_OFFSETOF(PostprocessController, m_DepthStencilTextures, 0x24);
    MARATHON_ASSERT_SIZEOF(PostprocessController, 0x3C);
}
