#pragma once

#include <Marathon.inl>
#include <api/stdx/map.h>
#include <api/stdx/string.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/Graphics/Xenon/SurfaceXenon.h>
#include <Sonicteam/SoX/Graphics/Xenon/TextureXenon.h>
#include <Sonicteam/SoX/Graphics/Device.h>

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
        stdx::map<stdx::string, SoX::RefSharedPointer<SoX::Graphics::Xenon::SurfaceXenon>> m_mspDepthStencill_1_4; // m_mspDepthStencill_1_4 ??
        stdx::map<stdx::string, SoX::RefSharedPointer<SoX::Graphics::Xenon::SurfaceXenon>> m_mspDepthStencill_256;
        stdx::map<stdx::string, SoX::RefSharedPointer<SoX::Graphics::Xenon::TextureXenon>> m_mspFrameBuffer;
        stdx::map<stdx::string, SoX::RefSharedPointer<SoX::Graphics::Xenon::TextureXenon>> m_mspPostEffect; //relection, csm, radermap,... better name ?
        stdx::map<stdx::string, SoX::RefSharedPointer<SoX::Graphics::Xenon::TextureXenon>> m_mspPostEffectAfter; //radermap_mask, better name ?
    };
}
