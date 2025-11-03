#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Xenon/SurfaceXenon.h>
#include <Sonicteam/SoX/Graphics/Xenon/TextureXenon.h>
#include <Sonicteam/SoX/Graphics/Device.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <api/stdx/map.h>
#include <api/stdx/string.h>

namespace Sonicteam
{
    class RenderTargetContainer
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
        };

        xpointer<Vftable> m_pVftable;
        xpointer<Sonicteam::SoX::Graphics::Device> m_pDevice;
        stdx::map<stdx::string, SoX::RefSharedPointer<SoX::Graphics::Xenon::SurfaceXenon>> m_mspDepthStencil_1_4;
        stdx::map<stdx::string, SoX::RefSharedPointer<SoX::Graphics::Xenon::SurfaceXenon>> m_mspDepthStencil_256;
        stdx::map<stdx::string, SoX::RefSharedPointer<SoX::Graphics::Xenon::TextureXenon>> m_mspFrameBuffer;
        stdx::map<stdx::string, SoX::RefSharedPointer<SoX::Graphics::Xenon::TextureXenon>> m_mspPostEffect;      // reflection, csm, radermap
        stdx::map<stdx::string, SoX::RefSharedPointer<SoX::Graphics::Xenon::TextureXenon>> m_mspPostEffectAfter; // radermap_mask
    };
}
