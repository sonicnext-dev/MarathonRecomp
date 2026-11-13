#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/Player/State/CommonContext.h>
#include <Sonicteam/Player/SonicGauge.h>

namespace Sonicteam::Player::State
{
    class SonicContext : public CommonContext
    {
    public:
        enum GemSprite : uint32_t
        {
            GemSprite_Green = 1,
            GemSprite_Red,
            GemSprite_Blue,
            GemSprite_White,
            GemSprite_Sky,
            GemSprite_Yellow,
            GemSprite_Purple,
            GemSprite_Super
        };

        enum Gem : uint32_t
        {
            Gem_Blue = 1,
            Gem_Red,
            Gem_Green,
            Gem_Purple,
            Gem_Sky,
            Gem_White,
            Gem_Yellow,
            Gem_Super
        };

        static constexpr GemSprite ms_GemSpriteConversionTable[8] =
        {
            GemSprite_Blue,
            GemSprite_Red,
            GemSprite_Green,
            GemSprite_Purple,
            GemSprite_Sky,
            GemSprite_White,
            GemSprite_Yellow,
            GemSprite_Super
        };

        be<GemSprite> m_CurrentGemSprite;
        boost::shared_ptr<SonicGauge> m_spGauge;
        bool m_IsHoming;
        bool m_IsLightDash;
        bool m_IsSlidingAttack;
        bool m_Field23F;
        bool m_IsBoundAttack;
        bool m_Field241;
        bool m_IsPurpleGem;
        bool m_IsThunderGuard;
        bool m_IsTornado;
        bool m_IsGunDriveCharge;
        bool m_IsGunDriveThrow;
        bool m_IsRedGem;
        bool m_IsBlueAura;
        bool m_IsGemsEnabled;
        bool m_Field24A;
        bool m_Field24B;
        be<uint32_t> m_HomingIndex;
        be<Gem> m_CurrentGem;
        MARATHON_INSERT_PADDING(0x58);
    };

    MARATHON_ASSERT_OFFSETOF(SonicContext, m_CurrentGemSprite, 0x230);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_spGauge, 0x234);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_IsHoming, 0x23C);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_IsLightDash, 0x23D);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_IsSlidingAttack, 0x23E);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_Field23F, 0x23F);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_IsBoundAttack, 0x240);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_Field241, 0x241);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_IsPurpleGem, 0x242);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_IsThunderGuard, 0x243);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_IsTornado, 0x244);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_IsGunDriveCharge, 0x245);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_IsGunDriveThrow, 0x246);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_IsRedGem, 0x247);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_IsBlueAura, 0x248);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_IsGemsEnabled, 0x249);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_Field24A, 0x24A);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_Field24B, 0x24B);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_HomingIndex, 0x24C);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_CurrentGem, 0x250);
    MARATHON_ASSERT_SIZEOF(SonicContext, 0x2B0);
}
