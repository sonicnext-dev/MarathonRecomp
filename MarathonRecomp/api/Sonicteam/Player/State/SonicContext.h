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
        be<float> c_homing_spd;
        be<float> c_homing_brake;
        be<float> c_sliding_time;
        be<float> c_spindash_spd;
        be<float> c_spindash_time;
        be<float> c_bound_jump_spd_0;
        be<float> c_bound_jump_spd_1;
        be<float> c_lightdash_speed;
        be<float> c_boundjump_jmp;
        be<float> c_boundjump_block;
        be<float> c_attack_brake;
        be<float> c_sliding_speed_min;
        be<float> c_sliding_speed_max;
        be<float> c_homing_smash_charge;
        be<float> c_custom_action_slow_time;
        be<float> c_custom_action_machspeed_acc;
        be<float> c_custom_action_machspeed_time;
        be<float> c_scale_run_speed_max;
        be<float> c_scale_run_acc;
        be<float> c_scale_walk_speed_max;
        be<float> c_scale_jump_speed;
        be<float> c_scale_jump_block;
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
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_homing_spd, 0x254);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_homing_brake, 0x258);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_sliding_time, 0x25C);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_spindash_spd, 0x260);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_spindash_time, 0x264);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_bound_jump_spd_0, 0x268);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_bound_jump_spd_1, 0x26C);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_lightdash_speed, 0x270);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_boundjump_jmp, 0x274);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_boundjump_block, 0x278);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_attack_brake, 0x27C);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_sliding_speed_min, 0x280);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_sliding_speed_max, 0x284);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_homing_smash_charge, 0x288);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_custom_action_slow_time, 0x28C);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_custom_action_machspeed_acc, 0x290);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_custom_action_machspeed_time, 0x294);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_scale_run_speed_max, 0x298);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_scale_run_acc, 0x29C);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_scale_walk_speed_max, 0x2A0);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_scale_jump_speed, 0x2A4);
    MARATHON_ASSERT_OFFSETOF(SonicContext, c_scale_jump_block, 0x2A8);
    MARATHON_ASSERT_SIZEOF(SonicContext, 0x2B0);
}
