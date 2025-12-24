#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Model.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <stdx/map.h>
#include <stdx/string.h>


namespace Sonicteam::SoX::Graphics
{
    class Texture;

    class SamplerCommon
    {
    public:
        SoX::RefSharedPointer<Texture> m_spTexture;
        MARATHON_INSERT_PADDING(8);
        be<uint32_t> m_TextureRefs; // refCount of m_spTexture
        MARATHON_INSERT_PADDING(8);
    };

    MARATHON_ASSERT_OFFSETOF(SamplerCommon, m_spTexture, 0x0);
    MARATHON_ASSERT_SIZEOF(SamplerCommon, 0x18);
}
