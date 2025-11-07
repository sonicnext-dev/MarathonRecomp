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
            Wrap = 1,
            Clamp = 2,
            Mirror = 3,
            Border = 4,
        };

        enum FilterMode : uint32_t
        {
            Point = 1,
            Linear = 2,
            LinearMipmap = 3,
            Trilinear = 4,
            Anisotropic = 5,
            Gaussian = 5,
        };

        be<uint32_t> m_Unknown;
        xpointer<void> m_pTexture;
        be<AddressMode> m_XAddressMode;
        be<AddressMode> m_YAddressMode;
        be<FilterMode> m_FilterMode;
    };

    MARATHON_ASSERT_SIZEOF(SetTexture, 0x4C);
    MARATHON_ASSERT_OFFSETOF(SetTexture, m_Unknown, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetTexture, m_pTexture, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetTexture, m_XAddressMode, 0x40);
    MARATHON_ASSERT_OFFSETOF(SetTexture, m_YAddressMode, 0x44);
    MARATHON_ASSERT_OFFSETOF(SetTexture, m_FilterMode, 0x48);
}
