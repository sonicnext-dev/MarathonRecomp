#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetTexture : public MyRenderProcess
    {
    public:
        enum AddressMode : uint32_t
        {
            AddressMode_Wrap = 1,
            AddressMode_Clamp = 2,
            AddressMode_Mirror = 3,
            AddressMode_Border = 4,
        };

        enum FilterMode : uint32_t
        {
            FilterMode_Point = 1,
            FilterMode_Linear = 2,
            FilterMode_LinearMipmap = 3,
            FilterMode_Trilinear = 4,
            FilterMode_Anisotropic = 5,
            FilterMode_Gaussian = 5,
        };

        be<uint32_t> m_Unknown;
        xpointer<SoX::Graphics::Texture> m_pTexture;
        be<AddressMode> m_XAddressMode;
        be<AddressMode> m_YAddressMode;
        be<FilterMode> m_FilterMode;
    };

    MARATHON_ASSERT_OFFSETOF(SetTexture, m_Unknown, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetTexture, m_pTexture, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetTexture, m_XAddressMode, 0x40);
    MARATHON_ASSERT_OFFSETOF(SetTexture, m_YAddressMode, 0x44);
    MARATHON_ASSERT_OFFSETOF(SetTexture, m_FilterMode, 0x48);
    MARATHON_ASSERT_SIZEOF(SetTexture, 0x4C);
}
