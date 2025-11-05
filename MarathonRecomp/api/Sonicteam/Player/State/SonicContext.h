#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/State/CommonContext.h>
#include <Sonicteam/Player/SonicGauge.h>
#include <boost/smart_ptr/shared_ptr.h>

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
        boost::shared_ptr<SonicGauge> m_Gauge;
        uint8_t m_HomingLockOn;
        uint8_t m_DisablePlayerMovement;
        uint8_t m_AntigravityHitBox;
        uint8_t m_Field23F;
        uint8_t m_BoundAttackHitBox;
        uint8_t m_Field241;
        uint8_t m_Shrink;
        uint8_t m_ThunderGuard;
        uint8_t m_Tornado;
        uint8_t m_FPS;
        uint8_t m_ThrowGem;
        uint8_t m_SlowTime;
        uint8_t m_MachAura;
        uint8_t m_GemsEnabled;
        uint8_t m_Field24A;
        uint8_t m_Field24B;
        be<uint32_t> m_HomingFlip;
        be<Gem> m_CurrentGem;
        MARATHON_INSERT_PADDING(0x58);
    };

    MARATHON_ASSERT_OFFSETOF(SonicContext, m_CurrentGemSprite, 0x230);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_Gauge, 0x234);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_HomingLockOn, 0x23C);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_DisablePlayerMovement, 0x23D);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_AntigravityHitBox, 0x23E);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_Field23F, 0x23F);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_BoundAttackHitBox, 0x240);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_Field241, 0x241);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_Shrink, 0x242);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_ThunderGuard, 0x243);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_Tornado, 0x244);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_FPS, 0x245);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_ThrowGem, 0x246);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_SlowTime, 0x247);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_MachAura, 0x248);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_GemsEnabled, 0x249);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_Field24A, 0x24A);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_Field24B, 0x24B);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_HomingFlip, 0x24C);
    MARATHON_ASSERT_OFFSETOF(SonicContext, m_CurrentGem, 0x250);
    MARATHON_ASSERT_SIZEOF(SonicContext, 0x2B0);
}
