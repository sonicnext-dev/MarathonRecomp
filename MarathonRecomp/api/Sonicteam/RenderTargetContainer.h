#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Graphics/Surface.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/MyGraphicsDevice.h>
#include <Sonicteam/MyTexture.h>
#include <stdx/map.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class RenderTargetContainer
    {
    public:
        xpointer<void> m_pVftable;
        xpointer<MyGraphicsDevice> m_pDevice;
        stdx::map<stdx::string, SoX::RefSharedPointer<SoX::Graphics::Surface>> m_DepthStencilSurfaces;
        stdx::map<stdx::string, SoX::RefSharedPointer<SoX::Graphics::Surface>> m_DepthStencil256xSurfaces;
        stdx::map<stdx::string, SoX::RefSharedPointer<MyTexture>> m_FrameBufferTextures;
        stdx::map<stdx::string, SoX::RefSharedPointer<MyTexture>> m_PostEffectTextures;      // reflection, csm, radermap
        stdx::map<stdx::string, SoX::RefSharedPointer<MyTexture>> m_PostEffectAfterTextures; // radermap_mask
        xpointer<SoX::Engine::Doc> m_pDoc;
    };

    MARATHON_ASSERT_OFFSETOF(RenderTargetContainer, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(RenderTargetContainer, m_pDevice, 0x04);
    MARATHON_ASSERT_OFFSETOF(RenderTargetContainer, m_DepthStencilSurfaces, 0x08);
    MARATHON_ASSERT_OFFSETOF(RenderTargetContainer, m_DepthStencil256xSurfaces, 0x14);
    MARATHON_ASSERT_OFFSETOF(RenderTargetContainer, m_FrameBufferTextures, 0x20);
    MARATHON_ASSERT_OFFSETOF(RenderTargetContainer, m_PostEffectTextures, 0x2C);
    MARATHON_ASSERT_OFFSETOF(RenderTargetContainer, m_PostEffectAfterTextures, 0x38);
    MARATHON_ASSERT_OFFSETOF(RenderTargetContainer, m_pDoc, 0x44);
    MARATHON_ASSERT_SIZEOF(RenderTargetContainer, 0x48);
}
